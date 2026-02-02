#include "contactlist.hpp"

#include "contactgroupitem.hpp"
#include "contactitem.hpp"

namespace
{

// 通过名字获取联系人分组
ContactGroup getGroupByName(const QString& name)
{
  if (name.isEmpty())
  {
    return ContactGroup::Hash;
  }

  QChar first = name.at(0);
  char letter = PinyinUtils::GetFirstPinyinLetter(first);

  if (letter == '\0')
  {
    ushort unicode = first.unicode();
    if ((unicode >= 'A' && unicode <= 'Z') || (unicode >= 'a' && unicode <= 'z'))
    {
      letter = static_cast<char>(std::toupper(unicode));
    }
  }

  if (letter >= 'A' && letter <= 'Z')
  {
    return static_cast<ContactGroup>(letter - 'A' + static_cast<int>(ContactGroup::A));
  }

  return ContactGroup::Hash;
}

// 通过分组枚举获取分组名称
QString getGroupName(ContactGroup group)
{
  if (group == ContactGroup::NewFriends)
  {
    return "新的朋友";
  }
  if (group == ContactGroup::Hash)
  {
    return "#";
  }
  if (group >= ContactGroup::A && group <= ContactGroup::Z)
  {
    char letter = 'A' + (static_cast<int>(group) - static_cast<int>(ContactGroup::A));
    return {letter};
  }
  return "";
}

}  // namespace

ContactList::ContactList(QWidget* parent) : CustomListWidget(parent)
{
  addNewFriendsItem();
  addTestData();
}

void ContactList::AddContactItem(const QString& name, const QString& head)
{
  ContactGroup group = getGroupByName(name);

  // 确保分组表头存在
  ensureGroupExists(group);

  auto* list_item = new QListWidgetItem();
  list_item->setSizeHint(QSize(0, UIConstants::ContactUserItemHeight));

  auto* contact_widget = new ContactItem();
  contact_widget->SetName(name);
  contact_widget->SetHead(head);

  int row = getGroupEndRow(group);
  insertItem(row, list_item);
  setItemWidget(list_item, contact_widget);
}

void ContactList::ensureGroupExists(ContactGroup group)
{
  if (_group_items.contains(group))
  {
    return;
  }

  int insert_row = 0;
  for (int i = static_cast<int>(group) - 1; i >= 0; --i)
  {
    auto prev_group = static_cast<ContactGroup>(i);
    if (_group_items.contains(prev_group))
    {
      insert_row = getGroupEndRow(prev_group);
      break;
    }
  }

  auto* list_item = new QListWidgetItem();
  list_item->setSizeHint(QSize(0, UIConstants::ContactGroupItemHeight));

  auto* group_widget = new ContactGroupItem();
  group_widget->SetLabelName(getGroupName(group));

  insertItem(insert_row, list_item);
  setItemWidget(list_item, group_widget);

  _group_items[group] = list_item;
}

// 添加新的朋友分组项，此处为固定设计，不会变化，直接连接信号槽
void ContactList::addNewFriendsItem()
{
  ensureGroupExists(ContactGroup::NewFriends);

  auto* list_item = new QListWidgetItem();
  list_item->setSizeHint(QSize(0, UIConstants::ContactUserItemHeight));

  auto* contact_widget = new ContactItem();
  contact_widget->SetName("新的朋友");
  contact_widget->SetHead(":/icons/new_friend.png", QSize(35, 35));

  int row = this->row(_group_items[ContactGroup::NewFriends]) + 1;
  insertItem(row, list_item);
  setItemWidget(list_item, contact_widget);

  connect(this, &QListWidget::itemClicked, this,
          [this, list_item](QListWidgetItem* item)
          {
            if (item == list_item)
            {
              emit sig_new_friends_clicked();
            }
          });
}

int ContactList::getGroupEndRow(ContactGroup group)
{
  for (int i = static_cast<int>(group) + 1; i < static_cast<int>(ContactGroup::Count); ++i)
  {
    auto next_group = static_cast<ContactGroup>(i);
    if (_group_items.contains(next_group))
    {
      return this->row(_group_items[next_group]);
    }
  }

  return count();
}

void ContactList::addTestData()
{
  const QStringList heads = {":/avatar/head_1.jpg", ":/avatar/head_2.jpg", ":/avatar/head_3.jpg"};

  AddContactItem("Alice", heads[0]);
  AddContactItem("Amy", heads[1]);
  AddContactItem("Bob", heads[2]);
  AddContactItem("Charlie", heads[0]);
  AddContactItem("Lisa", heads[1]);
  AddContactItem("Zack", heads[2]);
  AddContactItem("123用户", heads[0]);
  AddContactItem("王码字", heads[0]);
  AddContactItem("张三", heads[0]);
  AddContactItem("李四", heads[0]);
  AddContactItem("刘子", heads[0]);
}
