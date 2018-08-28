////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	designPattern.cpp
//
// summary:	Implements the design pattern with C++
// include: 1.Singleton		2.Simple Factory	3.Factory Method	4.Abstrct Factory
//			5.Prototype		6.Chain of Responsibility	7.Command	8.Interpretor
//			9.Iterator		10.Mediator			11.Memento			12.Observer
//			13.State		14.Strategy			15.Template Method	16.Visitor
//			17.Adapter		18.Bridge			19.Composite		20.Decorator
//			21.Facade		22.Flyweight		23.Proxy
//	refer:	https://github.com/CyC2018/Design-Patterns
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <list>
using namespace std;

/**********************************************************************************************//**
// Singleton with lock
 **************************************************************************************************/
#include <mutex>
std::mutex mt;
class Singleton {
public:
	static Singleton* getInstance() {
		if (instance == nullptr) {
			std::unique_lock<mutex> lock(mt);
			if(instance == nullptr)
				return instance;
			unique_lock<mutex> unlock(mt);
		}
	}
private:
	Singleton() { // setup the constructor private

	}
	static Singleton *instance;
	
};
Singleton* Singleton::instance = new Singleton();


/**********************************************************************************************//**
// Simple Factory
 **************************************************************************************************/
enum CTYPE{ COREA, COREB};

class Product{
public:
	virtual void show() {};
};

class ProductA : public Product {
public:
	virtual void show() { cout << "Product A" << endl; }
};

class ProductB : public Product {
public:
	virtual void show() { cout << "Product B" << endl; }
};

class SimpleFactory {
public:
	Product SingleCoreCreate(enum CTYPE CORE) {
		if (CORE == COREA) {
			return ProductA();
		}
		else if (CORE == COREB) {
			return ProductB();
		}
		return Product();
	}
};


/**********************************************************************************************//**
// Factory Method
 **************************************************************************************************/

class Factory {
public:
	virtual Product* factoryMethod() = 0;
public:
	void doSomething() {
		Product* product = factoryMethod();
		// do something with prodcut
	}
};

class ConcreteFactoryA : public Factory {
public:
	virtual Product * factoryMethod() {
		return new ProductA();
	}
};

class ConcreteFactoryB : public Factory {
public:
	virtual Product * factoryMethod() {
		return new ProductB();
	}
};


/**********************************************************************************************//**
// Abstrract Factory 
 **************************************************************************************************/

class AbstractProductA {

};

class AbstractProductB {

};

class ProductA1 : public AbstractProductA {

};

class ProductA2 : public AbstractProductA {

};

class ProductB1 : public AbstractProductB {

};

class ProductB2 : public AbstractProductB {

};

class AbstractFactory {
public:
	virtual AbstractProductA* createProdcutA() = 0;
	virtual AbstractProductB* createProductB() = 0;
};

class ConcreteFactory1 : public AbstractFactory {
public:
	virtual AbstractProductA* createProdcutA() {
		return new ProductA1();
	}

	virtual AbstractProductB* createProductB() {
		return new ProductB1();
	}
};

class ConcreteFactory2 : public AbstractFactory {
public:
	virtual AbstractProductA* createProdcutA() {
		return new ProductA2();
	}

	virtual AbstractProductB* createProductB() {
		return new ProductB2();
	}
};

/**********************************************************************************************//**
// Prototype
 **************************************************************************************************/

class Prototype {
public:
	virtual Prototype* myClone() = 0;
};

class ConcretePrototype : public Prototype {
private:
	string filed;
public:
	ConcretePrototype(string in_filed) { // for convinience and elegance, the destructor is not written in this cpp example
		filed = in_filed;
	}

	virtual Prototype* myClone() {
		return &ConcretePrototype(filed);
	}

	string toString() {
		return filed;
	}
};

/**********************************************************************************************//**
// Chain of Responsibility
// Nginx代理模型，避免请求的发送者和接收者之间存在强耦合
 **************************************************************************************************/
enum RequestType{ type1, type2 };
class Request {
private:
	RequestType type;
	string name;

public:
	Request(RequestType in_type, string in_name) {
		type = in_type;
		name = in_name;
	}

	RequestType getType() {
		return type;
	}

	string getName() {
		return name;
	}
};

class Handler {
protected:
	Handler * successor;
public:
	Handler(Handler* in_successor) {
		successor = in_successor;
	}
protected:
	virtual void handleRequest(Request request) = 0; // 类型与handle隔离
};

void Handler::handleRequest(Request request) {

}

class ConcreteHandler1 : public Handler {
public:
	ConcreteHandler1(Handler* in_successor) : Handler(in_successor) {

	}

protected:
	virtual void handleRequest(Request request) {
		if (request.getType() == RequestType::type1) {
			cout << request.getName() + "is handle by ConcreteHandler1" << endl;
			return;
		}
		if (successor != nullptr) {
			Handler::handleRequest(request); //静态调用纯虚函数
		}
	}
};

class ConcreteHandler2 : public Handler {
public:
	ConcreteHandler2(Handler* in_successor) : Handler(in_successor) {

	}

protected:
	virtual void handleRequest(Request request) {
		if (request.getType() == RequestType::type2) {
			cout << request.getName() + "is handle by ConcreteHandler2" << endl;
			return;
		}
		if (successor != nullptr) {
			Handler::handleRequest(request); //静态调用纯虚函数
		}
	}
};

/**********************************************************************************************//**
// Command 
// 命令封装进对象中，使用命令来初始化其他对象，将命令对象放入队列中进行排队，或者对象的操作记录到日志中
 **************************************************************************************************/
class Command {
public:
	virtual void execute() = 0;
};

class Light {
public:
	void on() {
		cout << "Light is on." << endl;
	}
	void off() {
		cout << "Light is off." << endl;
	}
};

class LightOnCommand : public Command {
	Light light;

public:
	LightOnCommand(Light in_light) {
		light = in_light;
	}

	virtual void execute() {
		light.on();
	}
};

class LightOffCommand : public Command {
	Light light;
public:
	LightOffCommand(Light in_light) {
		light = in_light;
	}

	virtual void execute() {
		light.off();
	}
};

class Invoker {
private:
	Command **onCommands;
	Command **offCommands;
	const int slotNum = 7;

public:
	Invoker() {
		onCommands = new Command*[slotNum];
		offCommands = new Command*[slotNum];
	}

	~Invoker() {
		delete[] onCommands;
		delete[] offCommands;
	}

	void setOnCommand(Command* command, int slot) {
		onCommands[slot] = command;
	}

	void setOffCommand(Command* command, int slot) {
		offCommands[slot] = command;
	}

	void onButtonWasPushed(int slot) {
		onCommands[slot]->execute();
	}

	void offButtonWasPushed(int slot) {
		offCommands[slot]->execute();
	}
};

/*
void run() {
	Invoker invoker;
	Light light;
	Command* lightOnCommand = &LightOnCommand(light);
	Command* lightOffCommand = &LightOffCommand(light);
	invoker.setOnCommand(lightOnCommand, 0);
	invoker.setOffCommand(lightOffCommand, 0);
	invoker.onButtonWasPushed(0);
	invoker.offButtonWasPushed(0);
}
*/

/**********************************************************************************************//**
// Interpreter
// 语法解释器
 **************************************************************************************************/
class Expression {
public:
	virtual bool interpret(string str) = 0;
};
#include "Tokenizer.h"
class TerminalExpression : public Expression {
private:
	string literal;

public:
	TerminalExpression(string str) : literal(str) {

	}

	virtual bool interpret(string str) {
		Tokenizer st(str);
		while (string(st.next()) != "") {
			string test = st.next();
			if (test == literal)
				return true;
		}
		return false;
	}
};

class AndExpression : public Expression {
private:
	Expression *expression1;
	Expression *expression2;

public:
	AndExpression(Expression *in_expr1, Expression *in_expr2) : expression1(in_expr1), expression2(in_expr2){

	}

	virtual bool interpret(string str) {
		return expression1->interpret(str) && expression2->interpret(str);
	}
};

class OrExpression : public Expression {
private:
	Expression *expression1;
	Expression *expression2;

public:
	OrExpression(Expression *in_expr1, Expression *in_expr2) : expression1(in_expr1), expression2(in_expr2) {

	}

	virtual bool interpret(string str) {
		return expression1->interpret(str) || expression2->interpret(str);
	}
};

/**********************************************************************************************//**
// Iterator
// 按一定顺序访问数据结构，算法和对象分离
 **************************************************************************************************/
template <typename Item>
class Iterator{
public:
	virtual Item next() = 0;
	virtual bool hasNext() = 0;
};

template <typename Item>
class Aggregate {
	Iterator createIterator() = 0;
};

template <typename Item>
class ConcreteIterator : public Iterator<Item> {
private:
	Item * items;
	int position;
	int length;
public:
	ConcreteIterator() : items(nullptr), position(0), length(0) {}

	ConcreteIterator(Item * in_items, int len){
		items = in_items;
		length = len;
	}

	virtual Item next() {
		return items[position++];
	}

	virtual bool hasNext() {
		return position < length;
	}
};

template <typename Item>
class ConcreteAggregate : public Aggregate<Item> {
private:
	int *items;
	int length;
public:
	ConcreteAggregate(){
		length = 10;
		items = new int[length];
		for (int i = 0; i < 10; i++) {
			items[i] = i;
		}
	}
	~ConcreteAggregate() {
		delete items;
	}

	Iterator createIterator() {
		return ConcreteIterator<int>(items, length);
	}
};

/**********************************************************************************************//**
// Mediator
// 使用中介者模式可以将复杂的两两依赖的结构转成星型结构
 **************************************************************************************************/
class Mediator {
public:
	virtual void doEvent(string eventType) = 0;
};

class RobotService {
public:
	virtual void onEvent(Mediator* mediator) = 0;
};

class Alarm : public RobotService {
public:
	virtual void onEvent(Mediator* mediator) {
		mediator->doEvent("alarm");
	}

	void doAlarm() {
		cout << "doAlarm()" << endl;
	}
};

class CoffeePot : public RobotService {
public:
	virtual void onEvent(Mediator* mediator) {
		mediator->doEvent("coffeePot");
	}

	void doCoffeePot() {
		cout << "doCoffeePot()" << endl;
	}
};

class Calender : public RobotService {
public:
	virtual void onEvent(Mediator* mediator) {
		mediator->doEvent("calender");
	}

	void doCalender() {
		cout << "doCalender()" << endl;
	}
};

class Sprinkler : public RobotService {
public:
	virtual void onEvent(Mediator* mediator) {
		mediator->doEvent("sprinkler");
	}

	void doSprinkler() {
		cout << "doSprinkler()" << endl;
	}
};

class ConcreteMediator : public Mediator {
private:
	Alarm alarm;
	CoffeePot coffeePot;
	Calender calender;
	Sprinkler sprinkler;
public:
	ConcreteMediator(Alarm in_alarm, CoffeePot in_coffeePot, Calender in_calender, Sprinkler in_sprinkler) {
		alarm = in_alarm;
		coffeePot = in_coffeePot;
		calender = in_calender;
		sprinkler = in_sprinkler;
	}

	virtual void doEvent(string eventType) { // mediator's core action
		if ("alarm" == eventType) {
			// four concrete services
			alarm.doAlarm();
			coffeePot.doCoffeePot();
			calender.doCalender();
			sprinkler.doSprinkler();
		}
		else if ("coffeePot" == eventType) {
			//
		}
		else if ("calender" == eventType) {
			//
		}
		else if("sprinkler" == eventType){
			//
		}
	}
};

/**********************************************************************************************//**
// Memento
// 在不违反封装的情况下获得对象的状态，在必要时恢复原来的状态reset
 **************************************************************************************************/
class PreviousCalculationToOriginator {
public:
	virtual int getFirstNumber() = 0;
	virtual int getSecondNumber() = 0;
};

class PreviousCalculationToCareTaker {
	// no operations permitted for the careTaker
};

class PreviousCalculationImp : public PreviousCalculationToOriginator, public PreviousCalculationToCareTaker {
private:
	int firstNumber;
	int secondNumber;

public:
	PreviousCalculationImp(int firstNum, int secondNum) :firstNumber(firstNum), secondNumber(secondNum) {

	}

	virtual int getFirstNumber() {
		return firstNumber;
	}

	virtual int getSecondNumber() {
		return secondNumber;
	}
};


class Calculator {
	// Create Memento
	virtual PreviousCalculationToCareTaker* backupLastCalculation() = 0;

	// Set Memento
	virtual void restorePreviousCalculation(PreviousCalculationToCareTaker* memento) = 0;

	virtual int getCalculationResult() = 0;

	virtual void setFirstNumber(int FirstNumber) = 0;

	virtual void setSecondNumber(int SecondNumber) = 0;
};

class CalculatorImp : public Calculator {
private:
	int firstNumber;
	int secondNumber;

public:
	virtual PreviousCalculationToCareTaker* backupLastCalculation() {
		return &PreviousCalculationImp(firstNumber, secondNumber);
	}

	virtual void restorePreviousCalculation(PreviousCalculationToCareTaker* memento) {
		firstNumber = ((PreviousCalculationToOriginator*)memento)->getFirstNumber();
		secondNumber = ((PreviousCalculationToOriginator*)memento)->getSecondNumber();
	}

	virtual int getCalculationResult() {
		return firstNumber + secondNumber;
	}

	virtual void setFirstNumber(int firstNum) {
		firstNumber = firstNum;
	}

	virtual void setSecondNumber(int secondNum) {
		secondNumber = secondNum;
	}
};

/**********************************************************************************************//**
// Observer 
// 一对多依赖，当对象状态改变时，通知所有依赖
 **************************************************************************************************/
class Observer {
public:
	virtual void update(float temperssure, float humidity, float pressure) = 0;
};

class Subject {
public:
	virtual void resisterObserver(Observer* o) = 0;
	virtual void removeObserver(Observer* o) = 0;
	virtual void notifyObserver() = 0;
};

class WeatherData : public Subject {
private:
	list<Observer*> observers;
	float temperature;
	float humidity;
	float pressure;

public:
	WeatherData(): observers(list<Observer*>()), temperature(0), humidity(0), pressure(0){
	
	}

	void setMeasurements(float t, float h, float p) {
		temperature = t;
		humidity = h;
		pressure = p;
		notifyObserver();
	}

	virtual void resisterObserver(Observer* o) {
		observers.emplace_back(o);
	}

	virtual void removeObserver(Observer* o) {
		
		for (auto iter = observers.begin(); iter != observers.end(); iter++) {
			if ((*iter) == o) {
				observers.remove(o);
				break;
			}
		}
	}

	virtual void notifyObserver() {
		for (Observer* o : observers) {
			o->update(temperature, humidity, pressure);
		}
	}
};

class StatisticDisplay : public Observer {
public:
	StatisticDisplay(Subject* weatherData) {
		weatherData->resisterObserver(this);
	}

	virtual void update(float temperature, float humidity, float pressure) {
		cout << "StatisticDisplay.update: " + to_string(temperature) + " " + to_string(humidity) + " " + to_string(pressure) << endl;
	}
};

class CurrentConditionsDisplay : public Observer {
public:
	CurrentConditionsDisplay(Subject* weatherData) {
		weatherData->resisterObserver(this);
	}

	virtual void update(float temperature, float humidity, float pressure) {
		cout << "CurrentConditionsDisplay.update: " + to_string(temperature) + " " + to_string(humidity) + " " + to_string(pressure) << endl;
	}
};

/**********************************************************************************************//**
																								// Strategy
																								// 与工厂模式生产一组类对象不同的是，策略模式封装一组可以互相替代的算法族
																								**************************************************************************************************/
class QuackBehavior {
public:
	virtual void quack() = 0;
};

class Quack : public QuackBehavior {
public:
	virtual void quack() {
		cout << "quack!" << endl;
	}
};

class Squack : public QuackBehavior {
public:
	virtual void quack() {
		cout << "squack!" << endl;
	}
};

class Duck {
private:
	QuackBehavior * quackBehavior;
public:

	void performQuack() {
		if (quackBehavior != nullptr) {
			quackBehavior->quack();
		}
	}

	void setQuackBehavior(QuackBehavior* in_quackBehavior) {
		quackBehavior = in_quackBehavior;
	}
};

/*
void run(){
Duck duck;
duck.setQuackBehavior(&Squack());
duck.performQuack();
duck.setQuackBehavior(&Quack());
duck.performQuack();
}
*/


/**********************************************************************************************//**
// State 
// 允许对象在内部状态改变时改变它的行为，对象看起来好像修改了它所属的类
 **************************************************************************************************/
class State {
public:
	virtual void insertQuarter() = 0;
	virtual void ejectQuarter() = 0;
	virtual void turnCrank() = 0;
	virtual void dispense() = 0;
};

class SoldOutState;
class NoQuarterState;
class HasQuarterState;
class SoldState;
class GumballMachine {
private:
	State * soldOutState;
	State * noQuarterState;
	State * hasQuarterState;
	State * soldState;
	State * state;
	int count;
public:
	GumballMachine() :count(0), soldOutState(nullptr), noQuarterState(nullptr), hasQuarterState(nullptr), soldState(nullptr)
		, state(nullptr) {}

	GumballMachine(int numberGumball);// 解决循环引用且用到对象实例的问题
	/* {
		count = numberGumball;
		soldOutState = &SoldOutState(*this);
		noQuarterState = &NoQuarterState(*this);
		hasQuarterState = &HasQuarterState(*this);
		soldState = &SoldState(*this);

		if (numberGumball > 0)
			state = noQuarterState;
		else
			state = soldOutState;
	}*/
	void insertQuarter() {
		state->insertQuarter();
	}
	void ejectQuarter() {
		state->ejectQuarter();
	}
	void turnCrank() {
		state->turnCrank();
	}

	void setState(State * in_state) {
		state = in_state;
	}

	void releaseBall() {
		cout << "A gumball comes rolling out the slot..." << endl;
		if (count != 0)
			count -= 1;
	}

	State* getSoldOutState() {
		return soldOutState;
	}
	State * getNoQuarterState() {
		return noQuarterState;
	}
	State * getHasQuarterState() {
		return hasQuarterState;
	}
	State * getSoldState() {
		return soldState;
	}
	int getCount() {
		return count;
	}
};

class HasQuarterState : public State {
private:
	GumballMachine gumballMachine;

public:
	HasQuarterState(GumballMachine g) {
		gumballMachine = g;
	}

	virtual void insertQuarter() {
		cout << "You can't insert another quarter" << endl;
	}
	virtual void ejectQuarter() {
		cout << "Quarter returned" << endl;
		gumballMachine.setState(gumballMachine.getNoQuarterState());
	}
	virtual void turnCrank() {
		cout << "You turned..." << endl;
		gumballMachine.setState(gumballMachine.getSoldState());
	}
	virtual void dispense() {
		cout << "No gumball dispensed" << endl;
	}
};

class NoQuarterState : public State {
private:
	GumballMachine gumballMachine;

public:
	NoQuarterState(GumballMachine g) {
		gumballMachine = g;
	}

	virtual void insertQuarter() {
		cout << "You insert a quarter" << endl;
		gumballMachine.setState(gumballMachine.getHasQuarterState());
	}
	virtual void ejectQuarter() {
		cout << "You have't inserted a quarter" << endl;
	}
	virtual void turnCrank() {
		cout << "You turned, but there's no quarter" << endl;
	}
	virtual void dispense() {
		cout << "You need to pay first" << endl;
	}
};

class SoldOutState : public State {
private:
	GumballMachine gumballMachine;

public:
	SoldOutState(GumballMachine g) {
		gumballMachine = g;
	}

	virtual void insertQuarter() {
		cout << "You can't insert a quarter, the machine is sold out" << endl;
	}
	virtual void ejectQuarter() {
		cout << "You can't eject, you haven't inserted a quarter yet" << endl;
	}
	virtual void turnCrank() {
		cout << "You turned, but there're no gumballs" << endl;
	}
	virtual void dispense() {
		cout << "No gumball dispensed" << endl;
	}
};

class SoldState : public State {
private:
	GumballMachine gumballMachine;

public:
	SoldState(GumballMachine g) {
		gumballMachine = g;
	}

	virtual void insertQuarter() {
		cout << "Please wait, we're already giving you a gumball" << endl;
	}
	virtual void ejectQuarter() {
		cout << "Sorry, you already turned the crank" << endl;
	}
	virtual void turnCrank() {
		cout << "Turning twice doesn't get you another gumball ?.?" << endl;
	}
	virtual void dispense() {
		gumballMachine.releaseBall();
		if (gumballMachine.getCount() > 0) {
			gumballMachine.setState(gumballMachine.getNoQuarterState());
		}
		else {
			cout << "Oops, out of gumballs" << endl;
			gumballMachine.setState(gumballMachine.getSoldOutState());
		}
	}
};

GumballMachine::GumballMachine(int numberGumball) {
		count = numberGumball;
		soldOutState = &SoldOutState(*this);
		noQuarterState = &NoQuarterState(*this);
		hasQuarterState = &HasQuarterState(*this);
		soldState = &SoldState(*this);

		if (numberGumball > 0)
			state = noQuarterState;
		else
			state = soldOutState;
}
/**********************************************************************************************//**
// Template Method 
// 定义算法框架，并将一些步骤的实现延迟到之类
 **************************************************************************************************/
class CaffeineBeverage {
public:
	virtual void prepareRecipe() final {
		boilWater();
		brew();
		pourInCup();
		addCondiments();
	}

	virtual void brew() = 0;

	virtual void addCondiments() = 0;

	virtual void boilWater() {
		cout << "boilWater" << endl;
	}
	virtual void pourInCup() {
		cout << "pourInCup" << endl;
	}
};

class Coffee : public CaffeineBeverage {
	virtual void brew() {
		cout << "Coffee.brew" << endl;
	}

	virtual void addCondiments() {
		cout << "Coffee.addCondiments" << endl;
	}
};

class Tea : public CaffeineBeverage {
	virtual void brew() {
		cout << "Tea.brew" << endl;
	}

	virtual void addCondiments() {
		cout << "Tea.addCondiments" << endl;
	}
};

/**********************************************************************************************//**
// Visitor 
// 为一个对象结构增加新功能
 **************************************************************************************************/
class Item;
class Order;
class Customer;
class Visitor {
public:
	virtual void visit(Customer* customer) = 0;
	virtual void visit(Order* order) = 0;
	virtual void visit(Item* item) = 0;
};

class Element {
public:
	virtual void accept(Visitor* visitor) {};
};

class Item : public Element {
private:
	string name;
public:
	Item(string in_name) : name(in_name) {

	}

	string getName() {
		return name;
	}
public:
	virtual void accept(Visitor* visitor) {
		visitor->visit(this);
	}
};


class Order : public Element {
private:
	string name;
	list<Item> items;
public:
	Order(string in_name) : name(in_name) {
		items = list<Item>();
	}

	Order(string in_name, string itemName) : name(in_name) {
		this->addItem(Item(itemName));
	}

	string getName() {
		return name;
	}

	void addItem(Item item) {
		items.emplace_back(item);
	}


	virtual void accept(Visitor* visitor) {
		visitor->visit(this);
		for (Item item : items) {
			item.accept(visitor);
		}
	}
};

class Customer : public Element{
private:
	string name;
	list<Order> orders;
public:
	Customer(string in_name) : name(in_name){
		orders = list<Order>();
	}

	string getName() {
		return name;
	}

	void addOrder(Order order) {
		orders.emplace_back(order);
	}

public:
	virtual void accept(Visitor* visitor) {
		visitor->visit(this);
		for (Order order : orders) {
			order.accept(visitor);
		}
	}
};


class CustomerGroup {
	list<Customer> customers;
public:
	CustomerGroup() {
		customers = list<Customer>();
	}
	void accept(Visitor* visitor) {
		for (Customer customer : customers) {
			customer.accept(visitor);
		}
	}

	void addCustomer(Customer customer) {
		customers.emplace_back(customer);
	}
};

class GeneralReport : public Visitor {
private:
	int customersNumber;
	int ordersNumber;
	int itemsNumber;

public:
	GeneralReport() :customersNumber(0), ordersNumber(0), itemsNumber(0) {}
	virtual void visit(Customer* customer) {
		cout << customer->getName() << endl;
		customersNumber++;
	}
	virtual void visit(Order* order) {
		cout << order->getName() << endl;
		ordersNumber++;
	}
	virtual void visit(Item* item) {
		cout << item->getName() << endl;
		itemsNumber++;
	}
	void displayResults() {
		cout << "Number of customers: " << customersNumber << endl;
		cout << "Number of orders: " << ordersNumber << endl;
		cout << "Number of items: " << itemsNumber << endl;
	}
};

/*
void run() {
	Customer customer1 = Customer("customer1");//纯虚函数不能有实例，即abstract = 0，而interface非纯虚函数
	customer1.addOrder(Order("Order1", "item1"));
	customer1.addOrder(Order("Order2", "item1"));
	customer1.addOrder(Order("Order3", "item1"));

	Order order = Order("order_a");
	order.addItem(Item("item_a1"));
	order.addItem(Item("item_a2"));
	order.addItem(Item("item_a3"));

	Customer customer2 = Customer("customer2");
	customer2.addOrder(order);

	CustomerGroup customers = CustomerGroup();
	customers.addCustomer(customer1);
	customers.addCustomer(customer2);

	GeneralReport visitor = GeneralReport();
	customers.accept(&visitor);
	visitor.displayResults();
}
*/


/**********************************************************************************************//**
// Adapter 
// 把一个类的接口转换成用户需要的接口
 **************************************************************************************************/
class Cock {
public:
	virtual void cock() = 0;
};
class Turkey {
public:
	virtual void gobble() = 0;
};

class WildTurkey : public Turkey {
public:
	virtual void gobble() {
		cout << "gobble!" << endl;
	}
};

class TurkeyAdapter : public Cock { // 接口转成Turkey like对象
	Turkey *turkey;
public:
	TurkeyAdapter(Turkey *in_turkey) {
		turkey = in_turkey;
	}

	virtual void cock() {
		turkey->gobble();
	}
};
/*
void run() {
	Turkey* turkey = &WildTurkey();
	Cock* cock = &TurkeyAdapter(turkey);
	cock->cock(); // 公鸡模仿火鸡
}
*/

/**********************************************************************************************//**
// Bridge 
// 接口与实现分离
 **************************************************************************************************/
class TV {
public:
	virtual void on() {};
	virtual void off() {};
	virtual void tuneChannel() {};
};

class Sony : public TV {
	virtual void on() {
		cout << "Sony.on()" << endl;
	}

	virtual void off() {
		cout << "Sony.off()" << endl;
	}

	virtual void tuneChannel() {
		cout << "Sony.tuneChannel()" << endl;
	}
};

class RCA : public TV {
	virtual void on() {
		cout << "RCA.on()" << endl;
	}

	virtual void off() {
		cout << "RCA.off" << endl;
	}

	virtual void tuneChannel() {
		cout << "RCA.tuneChannel()" << endl;
	}
};

class RemoteControl {
protected:
	TV tv;

public:
	RemoteControl(TV in_tv) {
		tv = in_tv;
	}

	virtual void on() {};

	virtual void off() {};

	virtual void tuneChannel() {};
};

class ConcreteRemoteControl1 : public RemoteControl {
public:
	ConcreteRemoteControl1(TV in_tv) : RemoteControl(in_tv){}

	virtual void on() {
		cout << "ConcreteRemoteControl1.on()" << endl;
		tv.on();
	}

	virtual void off() {
		cout << "ConcreteRemoteControl1.off()" << endl;
		tv.off();
	}

	virtual void tuneChannel() {
		cout << "ConcreteRemoteControl1.tuneChannel()" << endl;
		tv.tuneChannel();
	}
};

class ConcreteRemoteControl2 : public RemoteControl {
public:
	ConcreteRemoteControl2(TV in_tv) : RemoteControl(in_tv) {}

	virtual void on() {
		cout << "ConcreteRemoteControl2.on()" << endl;
		tv.on();
	}

	virtual void off() {
		cout << "ConcreteRemoteControl2.off()" << endl;
		tv.off();
	}

	virtual void tuneChannel() {
		cout << "ConcreteRemoteControl2.tuneChannel()" << endl;
		tv.tuneChannel();
	}
};

/*
void run() {
	RemoteControl remoteControl1 = ConcreteRemoteControl1(RCA());
	remoteControl1.on();
	remoteControl1.off();
	remoteControl1.tuneChannel();
}
*/

/**********************************************************************************************//**
// Composite
// 将对象组合成树形结构来表示"整体/部分"层次关系
 **************************************************************************************************/
class Component {
protected:
	string name;

public:
	Component(){}
	Component(string in_name) {
		name = in_name;
	}

	bool operator== (Component in_component) {
		return name == in_component.name;
	}

	void print() {
		cout << 0 << endl;
	}

	virtual void print(int level) {

	}

	virtual void add(Component component) {

	}

	virtual void remove(Component component) {

	}
};

class Composite : public Component {
private:
	list<Component> child;

public:
	Composite(string in_name) : Component(in_name){
		child = list<Component>();
	}

	virtual void print(int level) {
		for (int i = 0; i < level; i++) {
			cout << "--" << endl;
		}
		cout << "Composite:" + name << endl;
		for (Component component : child) {
			component.print(level + 1);
		}
	}

	virtual void add(Component component) {
		child.emplace_back(component);
	}

	virtual void remove(Component component) {
		child.remove(component);
	}
};

class Leaf : public Component {
public:
	Leaf(string in_name) : Component(in_name){}

	virtual void print(int level) {
		for (int i = 0; i < level; i++) {
			cout << "--" << endl;
		}
		cout << "left:" + name << endl;
	}

};

/**********************************************************************************************//**
// Decorator
// 为对象动态添加功能，即装饰者套在被装饰对象上
 **************************************************************************************************/
class Beverage {
public:
	virtual double cost() = 0;
};

class DarkRoast : public Beverage {
	virtual double cost() {
		return 1;
	}
};

class HouseBlend :public Beverage{
	virtual double cost() {
		return 1.2;
	}
};

class CondimentDecorator : public Beverage { // Decorator
protected:
	Beverage * beverage;
};

class Milk : public CondimentDecorator {
public:
	Milk(Beverage * bev) {
		beverage = bev;
	}

	virtual double cost() {
		return 1 + beverage->cost();
	}
};

class Mocha : public CondimentDecorator {
public:
	Mocha(Beverage * bev) {
		beverage = bev;
	}

	virtual double cost() {
		return 1 + beverage->cost();
	}
};

/*
void run() {
	Beverage * beverage = &HouseBlend();
	beverage = &Mocha(beverage);
	beverage = &Milk(beverage);
	cout << beverage->cost() << endl; // 3.0
}
*/

/**********************************************************************************************//**
// Facade
// 提供一个统一的接口，来访问子对象的一群接口，如实现一键看电影
 **************************************************************************************************/
class SubSystem {
public:
	void turnOnTV() {
		cout << "turnOnTV" << endl;
	}

	void setCD(string cd) {
		cout << "setCD( " + cd + " )" << endl;
	}

	void startWatching() {
		cout << "startWatching()" << endl;
	}
};

class Facade {
private:
	SubSystem subSystem;

public:
	void watchMovie() {
		subSystem.turnOnTV();
		subSystem.setCD("Impossible Mission");
		subSystem.startWatching();
	}
};

/**********************************************************************************************//**
// Flyweight
// 利用共享一个元类，避免拥有大量相同内容时小类的庞大开销，含内部和外部状态
 **************************************************************************************************/
class Flyweight {
public:
	virtual void doOperation(string extrinsicState) {};
};

class ConcreteFlyweight : public Flyweight {
private:
	string intrinsicState;

public:
	ConcreteFlyweight(string in_intrinsicState) : intrinsicState(in_intrinsicState) {}

	virtual void doOperation(string extrinsicState) {
		cout << "Object address: " << this << endl;
		cout << "IntrinsicState: " << intrinsicState << endl;
		cout << "ExtrinsicState: " << extrinsicState << endl;
	}
};

#include <unordered_map>

class FlyweightFactory {
private:
	unordered_map<string, Flyweight> flyweights;

public:
	FlyweightFactory() {
		flyweights = unordered_map<string, Flyweight>();
	}
	Flyweight getFlyweight(string in_intrinsicState) {
		if (flyweights.find(in_intrinsicState) == flyweights.end()) {
			Flyweight flyweight = ConcreteFlyweight(in_intrinsicState);
			flyweights.insert({ in_intrinsicState, flyweight });
		}
		return flyweights[in_intrinsicState];
	}
};

/*
void run() {
	FlyweightFactory factory;
	Flyweight flyweight1 = factory.getFlyweight("aa");
	Flyweight flyweight2 = factory.getFlyweight("aa");
	flyweight1.doOperation("x");
	flyweight2.doOperation("y");
}
*/

/**********************************************************************************************//**
// Proxy 
// 控制对其他对象的访问
 **************************************************************************************************/
class Image {
	virtual void showImage() {};
};

#include <time.h>
class HighResolutitonImage : public Image {
private:
	string imageURL;
	time_t startTime;
	int height;
	int width;

public:
	int getHeight() {
		return height;
	}

	int getWidth() {
		return width;
	}
	HighResolutitonImage() {}
	HighResolutitonImage(string imgURL) : imageURL(imgURL),startTime(clock()), height(600), width(600){

	}

	bool isLoad(){
		time_t endTime = clock();
		return endTime - startTime > 3000;
	}

	virtual void showImage() {
		cout << "Real Image: " << imageURL << endl;
	}
};
#include <windows.h>
class ImageProxy : public Image {
private:
	HighResolutitonImage highResolutionImage;

	ImageProxy(HighResolutitonImage in_highResolutionImage) {
		highResolutionImage = in_highResolutionImage;
	}

	virtual void showImage() {
		while (!highResolutionImage.isLoad()) {
			try {
				cout << "Temp Image: " << highResolutionImage.getWidth() << " " << highResolutionImage.getHeight() << endl;
				Sleep(100);
			}
			catch (exception e) {
				cout << e.what() << endl;
			}
		}
		highResolutionImage.showImage();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// End of designPattern.cpp
////////////////////////////////////////////////////////////////////////////////////////////////////

