#include "main.h"
bool shorter = false;
bool success = false;
// Balance
enum BalanceValue
{
	LH = -1,
	EH = 0,
	RH = 1
};
// Data exchange Rate
class dataEx{
public:
	int time;
	float BP;
	float AP;
	dataEx(){};
	~dataEx(){};
	dataEx(int _time, float _BP, float _AP){
		this->time = _time;
		this->BP = _BP;
		this->AP = _AP;
	}
};
// Exchange Rate
class exRate{
public:
	// Data
	dataEx data;
	// Pointer
	exRate *left;
	exRate *right;
	// Balance
	BalanceValue balance;
	// Constructor default
	exRate(){};
	// Constructor
	exRate(dataEx _dataEx){
		this->data = _dataEx;
		left = nullptr;
		right = nullptr;
		this->balance = EH;
	}
	// Destructor
	~exRate(){};
};
// Data of Currency Fair
class dataFair{
public:
	string BC;
	string QC;
	dataFair(){};
	~dataFair(){};
	dataFair(string _BC,string _QC){
		this->BC = _BC;
		this->QC = _QC;
	}
};
//Currency Fair
class currFair{
public:
// Attribute
	dataFair data;
	currFair *next;
	exRate *root;
	// Constructor default
	currFair(){};
	// Constructor
	currFair(dataFair _data){
		this->data = _data;
		root = nullptr;
		next = nullptr;
	}
	~currFair(){};
	// Method assitance
	exRate *rotateRight(exRate *&node);
	exRate *rotateLeft(exRate *&node);
	exRate *leftBalance(exRate *&node, bool &taller);
	exRate *rightBalance(exRate *&node, bool &taller);
	exRate *removeLeftBalance(exRate *&node, bool &shorter);
	exRate *removeRightBalance(exRate *&node, bool &shorter);
	// Method primary
	exRate *removeRec(exRate*&node, int &time, bool &shorter, bool &success);
	exRate *insertRec(exRate*&node, dataEx &_dataER, bool &taller);
	void remove(int*&,int&,exRate *&node,int _timeA,int _timeB);
	void clearRec(exRate *&node);
	void upLoad(exRate *&node,int _time,float _BP,float _AP,int &result);
	// Con trỏ save  lưu giữ node bằng time hay node gần nhất của nó luôn
	void findNode(exRate *node,int _time,exRate *&save);
};
// Linked list
class List{
public:
	currFair *head;
	int deposit;
	float leverage;
	int size=0;
	int *arrID = new int[1000];
	// Constructor
	List(){
		this->head = nullptr;
		deposit =0;
		leverage = 1;
	}
	// Destructor
	~List(){};
};
class nodeID{
public:
	nodeID *next;
	int ID;
	float cost;
	float scale;
	int BorS;
	string BC;
	string QC;
	nodeID(){};
	nodeID(int _ID,float _cost,float _scale,int _BorS,string _BC,string _QC){
		this->ID = _ID;
		this->cost = _cost;
		this->scale = _scale;
		this->BorS = _BorS;
		this->BC = _BC;
		this->QC = _QC;
		next = nullptr;
	}
	~nodeID(){};
};
class listID {
public:
	nodeID *phead;
	int count;
	listID(){
		phead = nullptr;
		count =0;
	}
	~listID(){};
};
class ProcessData : public currFair
{
private:
	List *pList;
	listID *pID;
	static const int MAXSIZECODE = 8;
	static int split(string line, string *&sp);
public:
	ProcessData();
	~ProcessData();
	int process(string line);
	int SD(const string *sp, const int n);
	int CD(const string *sp, const int n);
	int SL(const string *sp, const int n);
	int INS(const string *sp, const int n);
	int DEL(const string *sp, const int n);
	int UPD(const string *sp, const int n);
	int OB(const string *sp, const int n);
	int OS(const string *sp, const int n);
	int CB(const string *sp, const int n);
	int CS(const string *sp, const int n);
};
exRate *currFair::rotateRight(exRate *&node){
	exRate *temp = node->left;
	node->left = temp->right;
	temp->right = node;
	node = temp;
	return node;
}

exRate *currFair::rotateLeft(exRate *&node)
{
	exRate *temp = node->right;
	node->right = temp->left;
	temp->left = node;
	node = temp;
	return node;
}

exRate *currFair::leftBalance(exRate *&node, bool &taller){
	exRate *leftTree = node->left;
	if (leftTree->balance == LH)
	{
		node->balance = EH;
		leftTree->balance = EH;
		node = rotateRight(node);
		taller = false;
	}
	else
	{
		exRate *rightTree = leftTree->right;
		if (rightTree->balance == LH)
		{
			node->balance = RH;
			leftTree->balance = EH;
		}
		else if (rightTree->balance == EH)
		{
			node->balance = EH;
			leftTree->balance = EH;
		}
		else
		{
			node->balance = EH;
			leftTree->balance = LH;
		}

		rightTree->balance = EH;
		node->left = rotateLeft(leftTree);
		node = rotateRight(node);
		taller = false;
	}

	return node;
}

exRate *currFair::rightBalance(exRate *&node, bool &taller){
	exRate *rightTree = node->right;
	if (rightTree->balance == RH)
	{
		node->balance = EH;
		rightTree->balance = EH;
		node = rotateLeft(node);
		taller = false;
	}
	else
	{
		exRate *leftTree = rightTree->left;
		if (leftTree->balance == RH)
		{
			node->balance = LH;
			rightTree->balance = EH;
		}
		else if (leftTree->balance == EH)
		{
			node->balance = EH;
			rightTree->balance = EH;
		}
		else
		{
			node->balance = EH;
			rightTree->balance = RH;
		}

		leftTree->balance = EH;
		node->right = rotateRight(rightTree);
		node = rotateLeft(node);
		taller = false;
	}

	return node;
}

exRate *currFair::removeLeftBalance(exRate *&node, bool &shorter)
{
	if (node->balance == RH)
	{
		node->balance = EH;
	}
	else if (node->balance == EH)
	{
		node->balance = LH;
		shorter = false;
	}
	else
	{
		exRate *leftTree = node->left;
		if (leftTree->balance == RH)
		{
			exRate *rightTree = leftTree->right;
			if (rightTree->balance == RH)
			{
				leftTree->balance = LH;
				node->balance = EH;
			}
			else if (rightTree->balance == EH)
			{
				node->balance = EH;
				leftTree->balance = EH;
			}
			else
			{
				node->balance = RH;
				leftTree->balance = EH;
			}

			rightTree->balance = EH;
			node->left = rotateLeft(leftTree);
			node = this->rotateRight(node);
			shorter =false;
		}
		else
		{
			if (leftTree->balance != EH)
			{
				node->balance = EH;
				leftTree->balance = EH;
			}
			else
			{
				node->balance = LH;
				leftTree->balance = RH;
				shorter = false;
			}

			node = this->rotateRight(node);
		}
	}

	return node;
}

exRate *currFair::removeRightBalance(exRate *&node, bool &shorter)
{
	if (node->balance == LH)
	{
		node->balance = EH;
	}
	else if (node->balance == EH)
	{
		node->balance = RH;
		shorter = false;
	}
	else
	{
		exRate *rightTree = node->right;
		if (rightTree->balance == LH)
		{
			exRate *leftTree = rightTree->left;
			if (leftTree->balance == LH)
			{
				rightTree->balance = RH;
				node->balance = EH;
			}
			else if (leftTree->balance == EH)
			{
				node->balance = EH;
				rightTree->balance = EH;
			}
			else
			{
				node->balance = LH;
				rightTree->balance = EH;
			}

			leftTree->balance = EH;
			node->right = this->rotateRight(rightTree);
			node = this->rotateLeft(node);
			shorter =false;
		}
		else
		{
			if (rightTree->balance != EH)
			{
				node->balance = EH;
				rightTree->balance = EH;
			}
			else
			{
				node->balance = RH;
				rightTree->balance = LH;
				shorter = false;
			}

			node = this->rotateLeft(node);
		}
	}

	return node;
}

exRate *currFair::removeRec(exRate *&node, int &time, bool &shorter, bool &success)
{
	if (node == NULL)
	{
		shorter = false;
		success = false;
		return NULL;
	}

	if (time < node->data.time) {
		node->left = this->removeRec(node->left, time, shorter, success);

		if (shorter)
			node = removeRightBalance(node, shorter);
	}
	else if (time > node->data.time) {
		node->right = this->removeRec(node->right, time, shorter, success);

		if (shorter)
			node = removeLeftBalance(node, shorter);
	}
	else {
		exRate* removeNode = node;
		if (node->right == NULL) {
			exRate* newNode = node->left;
			success = true;
			shorter = true;
			delete removeNode;
			return newNode;
		}
		else if (node->left == NULL) {
			exRate* newNode = node->right;
			success = true;
			shorter = true;
			delete removeNode;
			return newNode;
		}
		else {
			exRate* exchPtr = node->left;
			while (exchPtr->right != NULL) {
				exchPtr = exchPtr->right;
			}
			node->data = exchPtr->data;
			node->left = this->removeRec(node->left, exchPtr->data.time, shorter, success);
			if (shorter)
				node = this->removeRightBalance(node, shorter);
		}
	}

	return node;
}
// Delete
void currFair::remove(int*& sp,int& count,exRate *&node,int timeA,int timeB){
	if (node == NULL) return;
	else {
		remove(sp,count,node->left,timeA,timeB);
		if (node->data.time >= timeA && node->data.time <= timeB){
			sp[count]=node->data.time;
			++count;
		}
		remove(sp,count,node->right,timeA,timeB);
	}
}
// Insert
exRate *currFair::insertRec(exRate*&node, dataEx &_dataER, bool &taller)
{
	if (node == NULL)
	{
		node = new exRate(_dataER);
		taller = true;
		return node;
	}

	if (_dataER.time < node->data.time)
	{
		node->left = insertRec(node->left, _dataER, taller);

		if (taller)
		{
			if (node->balance == LH)
			{
				node = leftBalance(node, taller);
				taller = false;
			}
			else if (node->balance == EH)
			{
				node->balance = LH;
			}
			else
			{
				node->balance = EH;
				taller = false;
			}
		}
	}
	else if (_dataER.time > node->data.time)
	{
		node->right = insertRec(node->right, _dataER, taller);

		if (taller)
		{
			if (node->balance == LH)
			{
				node->balance = EH;
				taller = false;
			}
			else if (node->balance == EH)
			{
				node->balance = RH;
			}
			else
			{
				node = rightBalance(node, taller);
				taller = false;
			}
		}
	}
	else if (_dataER.time == node->data.time)
	{
		node->data.AP = _dataER.AP;
		node->data.BP = _dataER.BP;
	}

	return node;
}
// Clear
void currFair::clearRec(exRate *&node)
{
	if (node != NULL)
	{
		this->clearRec(node->left);
		this->clearRec(node->right);
		delete node;
		node = NULL;
	}
}
void currFair::upLoad(exRate *&node,int _time,float _BP,float _AP,int &result){
	if (node == nullptr)	return ;
	if (_time < node->data.time)	upLoad(node->left,_time,_BP,_AP,result);
	else if (_time > node->data.time)	upLoad(node->right,_time,_BP,_AP,result);
	else {
		node->data.AP = _AP;
		node->data.BP = _BP;
		result =1;
	}
}
// Trả về 1 cái node có time như vậy hay gần nhất
void currFair::findNode(exRate *node,int _time,exRate *&save){
	if (node== nullptr)	return;
	if (_time< node->data.time)	findNode(node->left,_time,save);
	else if (_time> node->data.time){
		save = node;
		findNode(node->right,_time,save);
	}	
	else {
		save = node;
	}
}

