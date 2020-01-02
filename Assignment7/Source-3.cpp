// Copyright 2017, Bradley Peterson, Weber State University, all rights reserved.

#include <iostream>
#include <string>
#include <stack>
#include <sstream>
#include <cmath>

//To prevent those using g++ from trying to use a library
//they don't have
#ifndef __GNUC__
#include <conio.h>
#else
#include <stdio.h>
#endif

using std::stack;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::pow;

struct Node
{
	string data;
	Node* left{ nullptr };
	Node* right{ nullptr };
};

class TreeParser
{
public:
	TreeParser();
	void displayParseTree();
	void processExpression(string &expression);
	string computeAnswer();
	void insert(const string& item);
	bool exists(const string& item)const;
	void inOrderTraversal();
	void postOrderTraversal();
	void clear();

private:
	stack<string> mathStack;
	void initialize();
	double castStrToDouble(const string &s);
	string castDoubleToStr(const double d);
	void inOrderTraversal(Node * ptr);
	void postOrderTraversal(Node *ptr);
	void clear(Node * ptr);

	bool isDigit(char c);
	bool isOperator(char c);
	void processExpression(Node *p);
	void computeAnswer(Node *p);
	Node* root{ nullptr };

protected:
	string expression;
	int position;

};

TreeParser::TreeParser()
{
	initialize();
}

void TreeParser::displayParseTree()
{
	cout << "The expression seen using in-order traversal: ";
	inOrderTraversal();
	cout << endl;
	cout << "The expression seen using post-order traversal: ";
	postOrderTraversal();
	cout << endl;

}

void TreeParser::processExpression(string &expression)
{
	initialize();
	this->expression = expression;
	root = new Node;
	processExpression(root);

}

string TreeParser::computeAnswer()
{
	computeAnswer(root);
	return mathStack.top();
}

void TreeParser::insert(const string& item)
{
	if (root == nullptr)
	{
		root = new Node();
		root->data = item;
		return;
	}
	else {
		Node* curr = root;
		bool breakout = false;
		while (!breakout) {
			if (item > curr->data)
			{
				if (curr->right == nullptr)
				{
					curr->right = new Node();
					curr->right->data = item;
					breakout = true;
				}
				else
				{
					curr = curr->right;
				}
			}
			else
			{
				if (curr->left == nullptr)
				{
					curr->left = new Node();
					curr->left->data = item;
					breakout = true;
				}
				else
				{
					curr = curr->left;
				}

			}
		}
	}
}

bool TreeParser::exists(const string& item)const
{
	if (root == nullptr)
	{
		return false;
	}
	else
	{
		Node * curr = root;
		while (curr != nullptr && curr->data != item)
		{
			if (item > curr->data)
			{
				curr = curr->right;
			}
			else
			{
				curr = curr->left;
			}
		}
		return (curr != nullptr);
	}
}

void TreeParser::inOrderTraversal()
{
	inOrderTraversal(root);
}

void TreeParser::postOrderTraversal()
{
	postOrderTraversal(root);
}

void TreeParser::clear()
{
	clear(root);
}

void TreeParser::initialize()
{
	expression = "";
	position = 0;
	while (!mathStack.empty())
	{
		mathStack.pop();
	}

}

double TreeParser::castStrToDouble(const string &s)
{
	istringstream iss(s);
	double x;
	iss >> x;
	return x;
}

string TreeParser::castDoubleToStr(const double d)
{
	ostringstream oss;
	oss << d;
	return oss.str();

}

void TreeParser::inOrderTraversal(Node * ptr)
{
	if (ptr != nullptr) {

		inOrderTraversal(ptr->left);
		cout << ptr->data << " ";
		inOrderTraversal(ptr->right);
	}
}

void TreeParser::postOrderTraversal(Node * ptr)
{
	if (ptr != nullptr)
	{
		postOrderTraversal(ptr->left);
		postOrderTraversal(ptr->right);
		cout << ptr->data << " ";
	}
}

void TreeParser::clear(Node * ptr)
{
	if (ptr != nullptr) {
		clear(ptr->left);
		clear(ptr->right);
		delete ptr;
	}
}

bool TreeParser::isDigit(char c)
{
	if (c >= '0' && c <= '9')
	{
		return true;
	}
	else {
		return false;
	}
}

bool TreeParser::isOperator(char c)
{
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '^')
	{
		return true;
	}
	else {
		return false;
	}
}

void TreeParser::processExpression(Node *p)
{
	while (position < expression.size())
	{
		if (expression[position] == '(')
		{
			Node* temp = new Node;
			p->left = temp;
			temp->right = nullptr;
			temp->left = nullptr;
			position++;
			processExpression(temp);
		}
		else if (isDigit(expression[position]))
		{
			string temp = "";
			while (isDigit(expression[position]))
			{
				temp += expression[position];
				p->data = temp;
				position++;
			}
			return;
		}
		else if (isOperator(expression[position]))
		{
			Node* temp = new Node;
			p->data = expression[position];
			p->right = temp;
			temp->right = nullptr;
			temp->left = nullptr;
			position++;
			processExpression(temp);
		}
		else if (expression[position] == ')')
		{
			position++;
			return;
		}
		else if (expression[position] == ' ')
		{
			position++;
		}
	}
}

void TreeParser::computeAnswer(Node * p)
{
	if (p != NULL)
	{
		computeAnswer(p->left);
		computeAnswer(p->right);
		if (isDigit(p->data[0]))
		{
			mathStack.push(p->data);
		}

		else if (isOperator(p->data[0]))
		{
			double x = 0;
			double y = 0;
			double ans = 0;
			x = castStrToDouble(mathStack.top());
			mathStack.pop();
			y = castStrToDouble(mathStack.top());
			mathStack.pop();
			if (p->data[0] == '+')
			{
				ans = x + y;
				mathStack.push(castDoubleToStr(ans));
			}
			else if (p->data[0] == '*')
			{
				ans = x*y;
				mathStack.push(castDoubleToStr(ans));
			}
			else if (p->data[0] == '-')
			{
				ans = y - x;
				mathStack.push(castDoubleToStr(ans));
			}
			else if (p->data[0] == '^')
			{
				ans = pow(y, x);
				mathStack.push(castDoubleToStr(ans));
			}
			else if (p->data[0] == '/')
			{
				ans = y / x;
				mathStack.push(castDoubleToStr(ans));
			}
		}

	}
}

void pressAnyKeyToContinue()
{
	printf("Press any key to continue\n");

	//Linux and Mac users with g++ don't need this
	//But everyone else will see this message.
#ifndef __GNUC__
	_getch();
#else
	int c;
	fflush(stdout);
	do c = getchar(); while ((c != '\n') && (c != EOF));
#endif

}


int main() {
	TreeParser *tp = new TreeParser;

	string expression = "(4+7)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 11 as a double output

	expression = "(7-4)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 3 as a double output

	expression = "(4^3)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 64 as a double output

	expression = "((2-5)-5)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display -8 as a double output

	expression = "(5*(6/2))";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 15 as a double output

	expression = "((1 + 2) * (3 + 4))";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 21 as a double output

	expression = "(543+321)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display 864 as a double output

	expression = "(((((3+12)-7)*120)/(2+3))^3)";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display close to 7077888 as a double output

	expression = "(((((9+(2*(110-(30/2))))*8)+1000)/2)+(((3^4)+1)/2))";
	tp->processExpression(expression);
	tp->displayParseTree();
	cout << "The result is: " << tp->computeAnswer() << endl; //Should display close to 1337 as a double/decimal output

	pressAnyKeyToContinue();
	return 0;
}
