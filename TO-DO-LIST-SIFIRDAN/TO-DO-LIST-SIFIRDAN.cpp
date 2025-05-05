#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <conio.h>
#include <cstdlib>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <unordered_map>

using json = nlohmann::json;
using namespace std;

#define KEY_UP    72
#define KEY_DOWN  80
#define KEY_ENTER 13
#define KEY_RIGHT 77
#define KEY_LEFT  75
#define KEY_ESC   27

enum class Priority {
    Low = 0,
    Medium,
    High,
};

struct DateTime {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;

    DateTime() {
        year = 1900;
        month = 1;
        day = 1;
        hour = 0;
        minute = 0;
        second = 0;
    }

    DateTime(int y, int m, int d, int h, int min, int s)
        : year(y), month(m), day(d), hour(h), minute(min), second(s) {
    }

    DateTime(const tm& tm)
        : year(tm.tm_year + 1900), month(tm.tm_mon + 1), day(tm.tm_mday),
        hour(tm.tm_hour), minute(tm.tm_min), second(tm.tm_sec) {
    }

    DateTime(const DateTime& other) {
        year = other.year;
        month = other.month;
        day = other.day;
        hour = other.hour;
        minute = other.minute;
        second = other.second;
    }

    bool operator<(const DateTime& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        if (day != other.day) return day < other.day;
        if (hour != other.hour) return hour < other.hour;
        if (minute != other.minute) return minute < other.minute;
        return second < other.second;
    }

    bool operator>(const DateTime& other) const {
        return other < *this;
    }

    bool operator==(const DateTime& other) const {
        return year == other.year && month == other.month && day == other.day &&
            hour == other.hour && minute == other.minute && second == other.second;
    }

    friend ostream& operator<<(ostream& os, const DateTime& dt) {
        os << (dt.day < 10 ? "0" : "") << dt.day << "."
            << (dt.month < 10 ? "0" : "") << dt.month << "."
            << dt.year << " "
            << (dt.hour < 10 ? "0" : "") << dt.hour << ":"
            << (dt.minute < 10 ? "0" : "") << dt.minute << ":"
            << (dt.second < 10 ? "0" : "") << dt.second;
        return os;
    }

    int getdaysinmonth(int year, int month) const {
        if (month == 2) {
            return isleapyear(year) ? 29 : 28;
        }
        if (month == 4 || month == 6 || month == 9 || month == 11) {
            return 30;
        }
        return 31;
    }

    bool isleapyear(int year) const {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    bool isValid() const {
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > getdaysinmonth(year, month)) return false;
        if (hour < 0 || hour > 23) return false;
        if (minute < 0 || minute > 59) return false;
        if (second < 0 || second > 59) return false;
        return true;
    }

    void display() const {
        cout << (day < 10 ? "0" : "") << day << "."
            << (month < 10 ? "0" : "") << month << "."
            << year << " "
            << (hour < 10 ? "0" : "") << hour << ":"
            << (minute < 10 ? "0" : "") << minute << ":"
            << (second < 10 ? "0" : "") << second;
    }

    json toJson() const {
        return json{
            {"year", year},
            {"month", month},
            {"day", day},
            {"hour", hour},
            {"minute", minute},
            {"second", second}
        };
    }

    static DateTime fromJson(const json& j) {
        return DateTime(
            j["year"],
            j["month"],
            j["day"],
            j["hour"],
            j["minute"],
            j["second"]
        );
    }

    string format() const {
        stringstream ss;
        ss << year << "-"
            << (month < 10 ? "0" : "") << month << "-"
            << (day < 10 ? "0" : "") << day << " "
            << (hour < 10 ? "0" : "") << hour << ":"
            << (minute < 10 ? "0" : "") << minute;
        return ss.str();
    }

    static bool parseDateTime(const string& dateTimeStr, DateTime& dateTime) {
        istringstream ss(dateTimeStr);
        char delimiter;

        ss >> dateTime.year >> delimiter;
        if (delimiter != '-') return false;

        ss >> dateTime.month >> delimiter;
        if (delimiter != '-') return false;

        ss >> dateTime.day >> dateTime.hour >> delimiter;
        if (delimiter != ':') return false;

        ss >> dateTime.minute;
        dateTime.second = 0;

        return !ss.fail() && dateTime.isValid();
    }

    static bool isDateInRange(const DateTime& date, const DateTime& startDate, const DateTime& endDate) {
        return !(date < startDate) && !(endDate < date);
    }

    static DateTime getCurrentDate() {
        time_t now = time(0);
        tm currentDate;
        localtime_s(&currentDate, &now);
        return DateTime(currentDate);
    }

    static DateTime getEndDate(const DateTime& startDate, int days) {
        tm tmDate = {};
        tmDate.tm_year = startDate.year - 1900;
        tmDate.tm_mon = startDate.month - 1;
        tmDate.tm_mday = startDate.day;
        tmDate.tm_hour = startDate.hour;
        tmDate.tm_min = startDate.minute;
        tmDate.tm_sec = startDate.second;

        time_t startTime = mktime(&tmDate);
        startTime += days * 86400;

        tm endTm;
        localtime_s(&endTm, &startTime);

        return DateTime(endTm);
    }
};

class PriorityUtil {
public:
    static const string getName(Priority priority) {
        switch (priority) {
        case Priority::Low:    return "Low";
        case Priority::Medium: return "Medium";
        case Priority::High:   return "High";
        default:               return "Unknown";
        }
    }

    static Priority choosePriority() {
        int choice = 0;
        while (true) {
            system("cls || clear");
            cout << "Select task priority:\n";

            cout << (choice == 0 ? "\033[32mLow <<\033[0m\n" : "Low\n");
            cout << (choice == 1 ? "\033[32mMedium <<\033[0m\n" : "Medium\n");
            cout << (choice == 2 ? "\033[32mHigh <<\033[0m\n" : "High\n");

            int c = _getch();
            switch (c) {
            case KEY_UP:
                choice = (choice == 0) ? 2 : choice - 1;
                break;
            case KEY_DOWN:
                choice = (choice == 2) ? 0 : choice + 1;
                break;
            case KEY_ENTER:
            case KEY_RIGHT:
                return static_cast<Priority>(choice);
            case KEY_ESC:
            case KEY_LEFT:
                return static_cast<Priority>(choice);
            default:
                break;
            }
        }
    }
};

class Entity {
protected:
    size_t id;
    static size_t nextId;

public:
    Entity() : id(nextId++) {}
    virtual ~Entity() {}

    size_t getId() const { return id; }
    void setId(size_t newId) { id = newId; }

    static void updateNextId(size_t newNextId) {
        if (newNextId > nextId) {
            nextId = newNextId;
        }
    }
};

size_t Entity::nextId = 1;

class Task : public Entity {
private:
    string name;
    string description;
    Priority priority;
    DateTime completionDateTime;
    bool completed;

public:
    Task() : Entity(), priority(Priority::Low), completed(false) {
    }

    Task(const string& name, const string& description, Priority priority,
        const DateTime& completionDateTime, bool completed = false)
        : Entity(), name(name), description(description), priority(priority),
        completionDateTime(completionDateTime), completed(completed) {
    }

    const string& getName() const { return name; }
    const string& getDescription() const { return description; }
    Priority getPriority() const { return priority; }
    const DateTime& getCompletionDateTime() const { return completionDateTime; }
    bool isCompleted() const { return completed; }

    void setName(const string& name) { this->name = name; }
    void setDescription(const string& description) { this->description = description; }
    void setPriority(Priority priority) { this->priority = priority; }
    void setCompletionDateTime(const DateTime& completionDateTime) { this->completionDateTime = completionDateTime; }
    void setCompleted(bool completed) { this->completed = completed; }

    void toggleCompleted() {
        completed = !completed;
    }

    static Task* createFromInput() {
    Task* task = new Task();

    cout << "Enter task name: ";
    getline(cin, task->name);

    cout << "Enter task description: ";
    getline(cin, task->description);

    task->priority = PriorityUtil::choosePriority();

    while (true) {
        cout << "Enter completion date and time (YYYY-MM-DD HH:MM): ";
        string dateTimeStr;
        getline(cin, dateTimeStr);

        DateTime inputDateTime;
        if (!DateTime::parseDateTime(dateTimeStr, inputDateTime)) {
            cout << "Invalid date/time format. Please try again.\n";
            continue;
        }

        DateTime now = DateTime::getCurrentDate();
        if (inputDateTime < now) {
            cout << "Date/time cannot be in the past. Please enter a future date/time.\n";
            continue;
        }

        task->completionDateTime = inputDateTime;
        break;
    }

    task->completed = false;

    cout << endl;
    return task;
}


    void print() const {
        cout << "\n------ Task Info ------\n";
        cout << "Id:          " << id << endl;
        cout << "Name:        " << name << endl;
        cout << "Description: " << description << endl;
        cout << "Priority:    " << PriorityUtil::getName(priority) << endl;
        cout << "Due Date:    " << completionDateTime.format() << endl;
        cout << "Status:      " << (completed ? "Completed" : "Pending") << endl;
        cout << "----------------------\n";
    }

    void updateFromInput() {
        cout << "Enter new name (current: " << name << "): ";
        getline(cin, name);

        cout << "Enter new description (current: " << description << "): ";
        getline(cin, description);

        priority = PriorityUtil::choosePriority();

        while (true) {
            cout << "Enter new completion date and time (YYYY-MM-DD HH:MM) (current: "
                << completionDateTime.format() << "): ";
            string dateTimeStr;
            getline(cin, dateTimeStr);

            DateTime newDateTime;
            if (!DateTime::parseDateTime(dateTimeStr, newDateTime)) {
                cout << "Invalid date/time format. Please try again.\n";
                continue;
            }

            DateTime now = DateTime::getCurrentDate();
            if (newDateTime < now) {
                cout << "Date/time cannot be in the past. Please enter a future date/time.\n";
                continue;
            }

            completionDateTime = newDateTime;
            break;
        }

        cout << "Task updated successfully.\n";
    }

    bool containsText(const string& searchText) const {
        string lowerName = name;
        string lowerSearch = searchText;

        transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);

        return lowerName.find(lowerSearch) != string::npos;
    }

    json toJson() const {
        json taskJson;
        taskJson["id"] = id;
        taskJson["name"] = name;
        taskJson["description"] = description;
        taskJson["priority"] = static_cast<int>(priority);
        taskJson["completionDateTime"] = completionDateTime.toJson();
        taskJson["completed"] = completed;
        return taskJson;
    }

    static Task* fromJson(const json& taskJson) {
        Task* task = new Task();

        task->setId(taskJson["id"]);
        task->setName(taskJson["name"]);
        task->setDescription(taskJson["description"]);
        task->setPriority(static_cast<Priority>(taskJson["priority"].get<int>()));

        if (taskJson.contains("completionDateTime")) {
            DateTime dateTime = DateTime::fromJson(taskJson["completionDateTime"]);
            task->setCompletionDateTime(dateTime);
        }

        task->setCompleted(taskJson["completed"]);

        Entity::updateNextId(task->getId() + 1);

        return task;
    }
};

class TodoList : public Entity {
private:
    string name;
    string owner;
    vector<Task*> tasks;

public:
    TodoList() : Entity() {}

    TodoList(const string& name, const string& owner)
        : Entity(), name(name), owner(owner) {
    }

    ~TodoList() {
        for (Task* task : tasks) {
            delete task;
        }
    }

    const string& getName() const { return name; }
    const string& getOwner() const { return owner; }
    const vector<Task*>& getTasks() const { return tasks; }

    void setName(const string& name) { this->name = name; }
    void setOwner(const string& owner) { this->owner = owner; }

    static TodoList* create(const string& username) {
        TodoList* todoList = new TodoList("My Tasks", username);
        return todoList;
    }

    void addTask(Task* task) {
        tasks.push_back(task);
    }

    bool removeTask(size_t taskId) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if ((*it)->getId() == taskId) {
                delete* it;
                tasks.erase(it);
                return true;
            }
        }
        return false;
    }

    Task* findTaskById(size_t taskId) {
        for (Task* task : tasks) {
            if (task->getId() == taskId) {
                return task;
            }
        }
        return nullptr;
    }

    vector<Task*> findTasksByName(const string& name) {
        vector<Task*> result;
        for (Task* task : tasks) {
            if (task->containsText(name)) {
                result.push_back(task);
            }
        }
        return result;
    }

    vector<Task*> findTasksByPriority(Priority priority) {
        vector<Task*> result;
        for (Task* task : tasks) {
            if (task->getPriority() == priority) {
                result.push_back(task);
            }
        }
        return result;
    }

    vector<Task*> findTasksByDateRange(const DateTime& startDate, const DateTime& endDate) {
        vector<Task*> result;
        for (Task* task : tasks) {
            if (DateTime::isDateInRange(task->getCompletionDateTime(), startDate, endDate)) {
                result.push_back(task);
            }
        }
        return result;
    }

    vector<Task*> getTasksSortedByPriority() {
        vector<Task*> sortedTasks = tasks;
        sort(sortedTasks.begin(), sortedTasks.end(), [](Task* a, Task* b) {
            return static_cast<int>(a->getPriority()) > static_cast<int>(b->getPriority());
            });
        return sortedTasks;
    }

    vector<Task*> getTasksSortedByDate() {
        vector<Task*> sortedTasks = tasks;
        sort(sortedTasks.begin(), sortedTasks.end(), [](Task* a, Task* b) {
            return a->getCompletionDateTime() < b->getCompletionDateTime();
            });
        return sortedTasks;
    }

    void print() const {
        cout << "\n----------------------------\n";
        cout << "Todo List Info\n";
        cout << "Id:    " << id << endl;
        cout << "Name:  " << name << endl;
        cout << "Owner: " << owner << endl;

        cout << "\nTasks:\n";
        if (tasks.empty()) {
            cout << "No tasks found.\n";
        }
        else {
            for (Task* task : tasks) {
                task->print();
            }
        }
        cout << "----------------------------\n";
    }

    void printTasks(const vector<Task*>& taskList) const {
        if (taskList.empty()) {
            cout << "No tasks found.\n";
            return;
        }

        for (Task* task : taskList) {
            task->print();
        }
    }

    json toJson() const {
        json todoListJson;
        todoListJson["id"] = id;
        todoListJson["name"] = name;
        todoListJson["owner"] = owner;

        json tasksArray = json::array();
        for (const Task* task : tasks) {
            tasksArray.push_back(task->toJson());
        }
        todoListJson["tasks"] = tasksArray;

        return todoListJson;
    }

    void fromJson(const json& todoListJson) {
        for (Task* task : tasks) {
            delete task;
        }
        tasks.clear();

        id = todoListJson["id"];
        name = todoListJson["name"];
        owner = todoListJson["owner"];

        if (todoListJson.contains("tasks")) {
            for (const auto& taskJson : todoListJson["tasks"]) {
                Task* task = Task::fromJson(taskJson);
                tasks.push_back(task);
            }
        }

        Entity::updateNextId(id + 1);
    }

    bool saveToFile(const string& filename) const {
        json todoListJson = toJson();
        ofstream outputFile(filename);

        if (!outputFile.is_open()) {
            return false;
        }

        outputFile << todoListJson.dump(4);
        outputFile.close();
        return true;
    }

    bool loadFromFile(const string& filename) {
        ifstream inputFile(filename);

        if (!inputFile.is_open()) {
            return false;
        }

        try {
            json todoListJson;
            inputFile >> todoListJson;
            inputFile.close();

            fromJson(todoListJson);
            return true;
        }
        catch (const exception& e) {
            cerr << "Error loading todo list: " << e.what() << endl;
            return false;
        }
    }
};

class UserManager {
private:
    json usersData;
    string filename;
    unordered_map<string, TodoList*> userTodoLists;

public:
    UserManager(const string& file) : filename(file) {
        ifstream inputFile(filename);
        if (inputFile.is_open()) {
            inputFile >> usersData;
            inputFile.close();
        }
        else {
            usersData = { {"users", json::array()} };
        }
    }

    ~UserManager() {
        for (auto& pair : userTodoLists) {
            delete pair.second;
        }
    }

    void addUser(const string& username, const string& password) {
        json newUser = { {"username", username}, {"password", password} };
        usersData["users"].push_back(newUser);
        save();
    }

    bool userExists(const string& username, const string& password) const {
        for (auto& user : usersData["users"]) {
            if (user["username"] == username && user["password"] == password) {
                return true;
            }
        }
        return false;
    }

    void save() const {
        ofstream outputFile(filename);
        outputFile << usersData.dump(4);
        outputFile.close();
    }

    bool userExistsByUsername(const string& username) const {
        for (auto& user : usersData["users"]) {
            if (user["username"] == username) {
                return true;
            }
        }
        return false;
    }

    bool userMinNameLength(const string& username) const {
        return username.length() >= 3;
    }

    string getTaskFilename(const string& username) const {
        return username + "_tasks.json";
    }

    bool saveTodoList(const string& username) {
        auto it = userTodoLists.find(username);
        if (it != userTodoLists.end()) {
            string taskFilename = getTaskFilename(username);
            return it->second->saveToFile(taskFilename);
        }
        return false;
    }

    TodoList* getTodoList(const string& username) {
        auto it = userTodoLists.find(username);
        if (it != userTodoLists.end()) {
            return it->second;
        }

        TodoList* newList = TodoList::create(username);
        string taskFilename = getTaskFilename(username);

        if (!newList->loadFromFile(taskFilename) && newList->getTasks().empty()) {
        }

        userTodoLists[username] = newList;
        return newList;
    }
};

struct MenuItem {
    string name;
    vector<MenuItem> subMenus;
    int (*action)(void*);
};

class TaskManagementSystem {
private:
    TodoList* todoList;
    string username;
    UserManager* userManager;

    void saveChanges() {
        if (userManager) {
            userManager->saveTodoList(username);
        }
    }

    void addTask() {
        system("cls || clear");
        cout << "You selected: Add Task\n";
        Task* task = Task::createFromInput();
        todoList->addTask(task);
        cout << "Task added successfully!\n";
        saveChanges();
        system("pause");
    }

    void updateTask() {
        system("cls || clear");
        cout << "You selected: Update Task\n";
        size_t id;
        cout << "Enter the ID of the task you want to update: ";
        cin >> id;

        Task* task = todoList->findTaskById(id);
        if (task) {
            task->updateFromInput();
            saveChanges();
        }
        else {
            cout << "Task with ID " << id << " not found!\n";
        }
        system("pause");
    }

    void deleteTask() {
        system("cls || clear");
        cout << "You selected: Delete Task\n";
        size_t id;
        cout << "Enter the ID of the task you want to delete: ";
        cin >> id;

        if (todoList->removeTask(id)) {
            cout << "Task with ID " << id << " deleted successfully!\n";
            saveChanges();
        }
        else {
            cout << "Task with ID " << id << " not found!\n";
        }
        system("pause");
    }

    void searchTasksByName() {
        system("cls || clear");
        cout << "You selected: Search Tasks by Name\n";
        
        string searchText;
        cout << "Enter task name to search: ";
        getline(cin, searchText);

        vector<Task*> tasks = todoList->findTasksByName(searchText);
        if (tasks.empty()) {
            cout << "No tasks found with name containing '" << searchText << "'.\n";
        }
        else {
            cout << "Found " << tasks.size() << " tasks with names containing '" << searchText << "':\n";
            todoList->printTasks(tasks);
        }
        system("pause");
    }

    void searchTasksByPriority() {
        system("cls || clear");
        cout << "You selected: Search Tasks by Priority\n";
        cout << "Select the priority level:\n";
        Priority priority = PriorityUtil::choosePriority();

        vector<Task*> tasks = todoList->findTasksByPriority(priority);
        if (tasks.empty()) {
            cout << "No tasks found with " << PriorityUtil::getName(priority) << " priority.\n";
        }
        else {
            cout << "Tasks with " << PriorityUtil::getName(priority) << " priority:\n";
            todoList->printTasks(tasks);
        }
        system("pause");
    }

    void searchTasksByDateRange(int days, const string& period) {
        system("cls || clear");
        cout << "You selected: View Tasks for " << period << "\n";

        DateTime currentDate = DateTime::getCurrentDate();
        DateTime endDate = DateTime::getEndDate(currentDate, days);

        cout << "Showing tasks from " << currentDate.format()
            << " to " << endDate.format() << "\n\n";

        vector<Task*> tasks = todoList->findTasksByDateRange(currentDate, endDate);
        if (tasks.empty()) {
            cout << "No tasks found for this period.\n";
        }
        else {
            cout << "Found " << tasks.size() << " tasks:\n";
            todoList->printTasks(tasks);
        }
        system("pause");
    }

    void viewTasksForDay() {
        searchTasksByDateRange(1, "a Day");
    }

    void viewTasksForWeek() {
        searchTasksByDateRange(7, "a Week");
    }

    void viewTasksForMonth() {
        searchTasksByDateRange(30, "a Month");
    }

    void showAllTasksSortedByPriority() {
        system("cls || clear");
        cout << "You selected: Show All Tasks Sorted by Priority\n";
        vector<Task*> sortedTasks = todoList->getTasksSortedByPriority();
        if (sortedTasks.empty()) {
            cout << "No tasks found.\n";
        }
        else {
            cout << "Tasks sorted by priority (highest first):\n";
            todoList->printTasks(sortedTasks);
        }
        system("pause");
    }

    void showAllTasksSortedByDate() {
        system("cls || clear");
        cout << "You selected: Show All Tasks Sorted by Date\n";
        vector<Task*> sortedTasks = todoList->getTasksSortedByDate();
        if (sortedTasks.empty()) {
            cout << "No tasks found.\n";
        }
        else {
            cout << "Tasks sorted by completion date (earliest first):\n";
            todoList->printTasks(sortedTasks);
        }
        system("pause");
    }

    void toggleTaskStatus() {
        system("cls || clear");
        cout << "You selected: Toggle Task Status\n";
        size_t id;
        cout << "Enter the ID of the task you want to toggle: ";
        cin >> id;

        Task* task = todoList->findTaskById(id);
        if (task) {
            task->toggleCompleted();
            cout << "Task status toggled. New status: " << (task->isCompleted() ? "Completed"
                : "Pending") << ".\n";
            saveChanges();
        }
        else {
            cout << "Task with ID " << id << " not found!\n";
        }
        system("pause");
    }

    void printTodoList() {
        system("cls || clear");
        cout << "You selected: Print Todo List\n";
        todoList->print();
        system("pause");
    }

public:
    TaskManagementSystem(const string& username, UserManager* userManager)
        : username(username), userManager(userManager) {
        todoList = userManager->getTodoList(username);
    }

    ~TaskManagementSystem() {
    }

    void run() {
        vector<MenuItem> mainMenu = {
            {"Task Management", {
                {"Add New Task", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->addTask();
                    return 0;
                }},
                {"Update Task", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->updateTask();
                    return 0;
                }},
                {"Toggle Task Status", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->toggleTaskStatus();
                    return 0;
                }},
                {"Delete Task", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->deleteTask();
                    return 0;
                }}
            }, nullptr},
            {"View Tasks", {
                {"Print All Tasks", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->printTodoList();
                    return 0;
                }},
                {"Show Tasks Sorted by Priority", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->showAllTasksSortedByPriority();
                    return 0;
                }},
                {"Show Tasks Sorted by Date", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->showAllTasksSortedByDate();
                    return 0;
                }}
            }, nullptr},
            {"Search Tasks", {
                {"Search by Name", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->searchTasksByName();
                    return 0;
                }},
                {"Search by Priority", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->searchTasksByPriority();
                    return 0;
                }},
                {"View Tasks for Today", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->viewTasksForDay();
					return 0;
					}},
				{"View Tasks for This Week", {}, [](void* context) {
					static_cast<TaskManagementSystem*>(context)->viewTasksForWeek();
					return 0;
				}},
                {"View Tasks for This Month", {}, [](void* context) {
                    static_cast<TaskManagementSystem*>(context)->viewTasksForMonth();
                    return 0;
                }}
            }, nullptr},
            {"Save & Exit", {}, [](void* context) {
                static_cast<TaskManagementSystem*>(context)->saveChanges();
                return 1;
            }}
        };

        displayMenu(mainMenu, 0);
    }

private:
    void displayMenu(const vector<MenuItem>& menu, int parentIndex, int selectedIndex = 0) {
        int result = 0;
        while (result == 0) {
            system("cls || clear");
            cout << "===== Todo List Management System =====\n";
            cout << "User: " << username << "\n\n";

            for (size_t i = 0; i < menu.size(); ++i) {
                if (i == selectedIndex) {
                    cout << "\033[32m> " << menu[i].name << " <\033[0m\n";
                }
                else {
                    cout << "  " << menu[i].name << "\n";
                }
            }

            cout << "\nUse arrow keys to navigate, Enter to select, Esc to go back\n";

            int key = _getch();
            if (key == 224 || key == 0) {
                key = _getch();
            }

            switch (key) {
            case KEY_UP:
                selectedIndex = (selectedIndex > 0) ? selectedIndex - 1 : menu.size() - 1;
                break;
            case KEY_DOWN:
                selectedIndex = (selectedIndex < menu.size() - 1) ? selectedIndex + 1 : 0;
                break;
            case KEY_ENTER:
            case KEY_RIGHT:
                if (!menu[selectedIndex].subMenus.empty()) {
                    displayMenu(menu[selectedIndex].subMenus, selectedIndex);
                }
                else if (menu[selectedIndex].action != nullptr) {
                    result = menu[selectedIndex].action(this);
                }
                break;
            case KEY_ESC:
            case KEY_LEFT:
                if (parentIndex >= 0) {
                    return;
                }
                break;
            default:
                break;
            }
        }

        if (result == 1) {
            system("cls || clear");
            cout << "Thank you for using the Todo List Management System!\n";
            cout << "Your changes have been saved.\n";
            system("pause");
            exit(0);
        }
    }
};

class Application {
private:
    UserManager* userManager;

    bool login() {
        cout << "===== Login =====\n";
        string username, password;

        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;
        cin.ignore();

        if (userManager->userExists(username, password)) {
            cout << "\nLogin successful!\n";
            system("pause");
            TaskManagementSystem taskSystem(username, userManager);
            taskSystem.run();
            return true;
        }
        else {
            cout << "\nInvalid username or password!\n";
            system("pause");
            return false;
        }
    }

    bool register_user() {
        cout << "===== Register =====\n";
        string username, password, confirmPassword;

        cout << "Username (min 3 characters): ";
        cin >> username;

        if (!userManager->userMinNameLength(username)) {
            cout << "\nUsername must be at least 3 characters long!\n";
            system("pause");
            return false;
        }

        if (userManager->userExistsByUsername(username)) {
            cout << "\nUsername already exists!\n";
            system("pause");
            return false;
        }

        cout << "Password: ";
        cin >> password;
        cout << "Confirm Password: ";
        cin >> confirmPassword;

        if (password != confirmPassword) {
            cout << "\nPasswords do not match!\n";
            system("pause");
            return false;
        }

        userManager->addUser(username, password);
        cout << "\nRegistration successful!\n";
        system("pause");
        return true;
    }

public:
    Application() {
        userManager = new UserManager("users.json");
    }

    ~Application() {
        delete userManager;
    }

    void run() {
        bool exit = false;
        while (!exit) {
            cout << "===== Main Menu =====\n";

            vector<string> options = { "Login", "Register", "Exit" };
            int selectedOption = 0;

			bool insideMenu = true;
            while (insideMenu) {
                system("cls || clear");
                cout << "===== Main Menu =====\n";

                for (size_t i = 0; i < options.size(); ++i) {
                    if (i == selectedOption) {
                        cout << "\033[32m> " << options[i] << " <\033[0m\n";
                    }
                    else {
                        cout << "  " << options[i] << "\n";
                    }
                }

                cout << "\nUse arrow keys to navigate, Enter to select\n";

                int key = _getch();
                if (key == 224 || key == 0) { 
                    key = _getch();
                }

                switch (key) {
                case KEY_UP:
                    selectedOption = (selectedOption > 0) ? selectedOption - 1 : options.size() - 1;
                    break;
                case KEY_DOWN:
                    selectedOption = (selectedOption < options.size() - 1) ? selectedOption + 1 : 0;
                    break;
                case KEY_LEFT:
				case KEY_ESC:
                    exit = true;
                    insideMenu = false;
                    break;
                case KEY_ENTER:
				case KEY_RIGHT:
                    switch (selectedOption) {
                    case 0:
                        login();
                        break;
                    case 1:
                        register_user();
                        break;
                    case 2:
                        exit = true;
						insideMenu = false;
                        break;
                    }
                default:
                    break;
                }
            }
        }

        system("cls || clear");
        cout << "Thank you for using the Task Management System!\n";
        cout << "Goodbye!\n";
        system("pause");
    }
};

int main() {
	Application app;
	app.run();


    return 0;
}