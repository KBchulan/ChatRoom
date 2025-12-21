package main

import (
	"fmt"
	"log"
	"os"
	"os/exec"
	"os/signal"
	"path/filepath"
	"syscall"
	"time"

	"github.com/robfig/cron/v3"
	"gopkg.in/yaml.v2"
)

type Config struct {
	Database struct {
		Host     string `yaml:"host"`
		Port     int    `yaml:"port"`
		User     string `yaml:"user"`
		Password string `yaml:"password"`
		Name     string `yaml:"name"`
	} `yaml:"database"`
	Backup struct {
		Dir       string `yaml:"dir"`
		BinlogDir string `yaml:"binlog_dir"`
		KeepDays  int    `yaml:"keep_days"`
	} `yaml:"backup"`
}

var config Config

func main() {
	if err := loadConfig("config.yaml"); err != nil {
		log.Fatalf("Failed to load config: %v", err)
	}

	if err := os.MkdirAll(config.Backup.Dir, 0755); err != nil {
		log.Fatalf("Failed to create backup dir: %v", err)
	}
	if err := os.MkdirAll(filepath.Join(config.Backup.Dir, "binlog"), 0755); err != nil {
		log.Fatalf("Failed to create binlog dir: %v", err)
	}

	c := cron.New()

	// 每天 0 点执行
	_, err := c.AddFunc("0 0 * * *", func() {
		if time.Now().Weekday() == time.Sunday {
			log.Println("开始全量备份")
			if err := fullBackup(); err != nil {
				log.Printf("全量备份失败: %v", err)
			} else {
				log.Println("全量备份完成")
			}
		} else {
			log.Println("开始增量备份")
			if err := incrementalBackup(); err != nil {
				log.Printf("增量备份失败: %v", err)
			} else {
				log.Println("增量备份完成")
			}
		}
		// 清理过期备份
		cleanOldBackups()
	})

	if err != nil {
		log.Fatalf("Failed to add cron job: %v", err)
	}

	// 启动时检查是否有全量备份，没有则立即执行一次
	files, _ := filepath.Glob(filepath.Join(config.Backup.Dir, "full_*.sql"))
	if len(files) == 0 {
		log.Println("未发现全量备份，立即执行一次")
		if err := fullBackup(); err != nil {
			log.Printf("初始全量备份失败: %v", err)
		} else {
			log.Println("初始全量备份完成")
		}
	}

	c.Start()

	log.Println("备份服务已启动，等待定时任务中")

	quit := make(chan os.Signal, 1)
	signal.Notify(quit, syscall.SIGINT, syscall.SIGTERM)
	<-quit

	c.Stop()
	log.Println("备份服务已关闭")
}

func loadConfig(path string) error {
	data, err := os.ReadFile(path)
	if err != nil {
		return err
	}
	return yaml.Unmarshal(data, &config)
}

func fullBackup() error {
	filename := filepath.Join(config.Backup.Dir, fmt.Sprintf("full_%s.sql", time.Now().Format("20060102")))

	cmd := exec.Command("mysqldump",
		"-h", config.Database.Host,
		"-P", fmt.Sprintf("%d", config.Database.Port),
		"-u", config.Database.User,
		fmt.Sprintf("-p%s", config.Database.Password),
		"--single-transaction",
		"--master-data=2",
		"--flush-logs",
		config.Database.Name,
		"-r", filename,
	)

	output, err := cmd.CombinedOutput()
	if err != nil {
		return fmt.Errorf("%v: %s", err, output)
	}
	return nil
}

func incrementalBackup() error {
	cmd := exec.Command("mysql",
		"-h", config.Database.Host,
		"-P", fmt.Sprintf("%d", config.Database.Port),
		"-u", config.Database.User,
		fmt.Sprintf("-p%s", config.Database.Password),
		"-e", "FLUSH LOGS",
	)
	if output, err := cmd.CombinedOutput(); err != nil {
		return fmt.Errorf("flush logs 失败: %v: %s", err, output)
	}

	binlogFiles, err := filepath.Glob(filepath.Join(config.Backup.BinlogDir, "mysql-bin.[0-9][0-9][0-9][0-9][0-9][0-9]"))
	if err != nil {
		return err
	}

	destDir := filepath.Join(config.Backup.Dir, "binlog", time.Now().Format("20060102"))
	if err := os.MkdirAll(destDir, 0755); err != nil {
		return fmt.Errorf("创建 binlog 目录失败: %w", err)
	}

	for i := 0; i < len(binlogFiles)-1; i++ {
		src := binlogFiles[i]
		dst := filepath.Join(destDir, filepath.Base(src))
		if err := copyFile(src, dst); err != nil {
			log.Printf("复制 %s 失败: %v", src, err)
		}
	}
	return nil
}

func copyFile(src, dst string) error {
	input, err := os.ReadFile(src)
	if err != nil {
		return err
	}
	return os.WriteFile(dst, input, 0644)
}

func cleanOldBackups() {
	cutoff := time.Now().AddDate(0, 0, -config.Backup.KeepDays)

	files, _ := filepath.Glob(filepath.Join(config.Backup.Dir, "full_*.sql"))
	for _, f := range files {
		info, err := os.Stat(f)
		if err == nil && info.ModTime().Before(cutoff) {
			if err := os.Remove(f); err != nil {
				log.Printf("删除过期备份失败 %s: %v", f, err)
			} else {
				log.Printf("删除过期备份: %s", f)
			}
		}
	}

	binlogDirs, _ := filepath.Glob(filepath.Join(config.Backup.Dir, "binlog", "*"))
	for _, d := range binlogDirs {
		info, err := os.Stat(d)
		if err == nil && info.ModTime().Before(cutoff) {
			if err := os.RemoveAll(d); err != nil {
				log.Printf("删除过期 binlog 失败 %s: %v", d, err)
			} else {
				log.Printf("删除过期 binlog: %s", d)
			}
		}
	}
}
