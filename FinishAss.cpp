#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <math.h>

using namespace std;
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
// Trả về 1 cái node giữ giá trị của có time như vậy hay gần nhất
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

enum CodeValue
{
	sdCode,
	cdCode,
	slCode,
	insCode,
	delCode,
	updCode,
	obCode,
	cbCode,
	osCode,
	csCode
};

static map<string, CodeValue> s_mapCodeValues = {
	{"SD", sdCode},
	{"CD", cdCode},
	{"SL", slCode},
	{"INS", insCode},
	{"DEL", delCode},
	{"UPD", updCode},
	{"OB", obCode},
	{"CB", cbCode},
	{"OS", osCode},
	{"CS", csCode}};

ProcessData::ProcessData()
{
	pList = new List();
	pID = new listID();
}
ProcessData::~ProcessData()
{
	/*delete [] pID;
	delete [] pList;*/
}
// Check etablish deposit
int flagDeposit = 0;

// Split
int ProcessData::split(string line, string *&sp)
{
	sp = new string[MAXSIZECODE];
	const string SEP = " ";
	unsigned int pos=0, lastpos = 0, idx = 0;
	while (pos<= line.size() )
	{
		if (idx==0)
		{
			pos = line.find(SEP, lastpos);
			sp[idx] = line.substr(lastpos, pos - lastpos);
			++idx;
			lastpos = pos;	
		}
		else
		{
			pos = line.find(SEP, lastpos+1);
			sp[idx] = line.substr(lastpos+1, pos - lastpos-1);
			++idx;
			lastpos = pos;
		}
	}
	return idx;
}

int ProcessData::process(string line)
{

	string *p = new string[MAXSIZECODE];
	int n = ProcessData::split(line, p);
	if (n <= 0)
	{
		delete[] p;
		return -1;
	}
	int res = -1;
	try
	{
		switch (s_mapCodeValues[p[0]])
		{
		case sdCode:
		{
			res = this->SD(p,n);
			break;
		}
		case cdCode:
		{
			res = this->CD(p,n);
			break;
		}
		case slCode:
		{
			res = this->SL(p,n);
			break;
		}
		case insCode:
		{
			res = this->INS(p, n);
			break;
		}
		case delCode:
		{
			res = this->DEL(p,n);
			break;
		}
		case updCode:
		{
			res = this->UPD(p,n);
			break;
		}
		case obCode:
		{
			res = this->OB(p,n);
			break;
		}
		case osCode:
		{
			res = this->OS(p,n);
			break;
		}
		case cbCode:
		{
			res = this->CB(p,n);
			break;
		}
		case csCode:
		{
			res = this->CS(p,n);
			break;
		}
		default:
			res = -1;
		}
	}
	catch (invalid_argument iax)
	{
		delete[] p;
		return res;
	}
	delete[] p;
	return res;
}

// Instruction SD
int ProcessData::SD(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == ""){
				
				return -1;
			}
		}
		if (n>2|| n<2){	
			
			return -1;
		}
		// Data input
		int MN =  stoi(sp[1]);
		// Configure 
		if (pList->deposit == 0){
			pList->deposit = MN;
			flagDeposit = 1;
			
			return 1;
		}
		// Thay đổi giảm
		if (MN< pList->deposit){
			pList->deposit = MN;
			
			return 1;
		}
		// Thay đổi tăng
		if (MN > pList->deposit){
			int flag =0;
			// Kiểm tra có các giao dịch mở lệnh hay không
			for (int i = 0; i < pList->size; i++)
			{
				if (pList->arrID[i] != -27){
					flag =1;
					break;
				}
			}
			// Success
			if (flag==1){
				pList->deposit = MN;
				
				return 1;
			}
			// Fail
			if (flag==0){
				
				return -1;
			}
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
	
}

// Instruction CD
int ProcessData::CD(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	{
				
				return -1;
			}
		}
		if (n>1){	
			
			return -1;
		}
		// Xem lại các phần trả về
		
		return ceil(pList->deposit);
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}

// Instruction SL
int ProcessData::SL(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == ""){
				
				return -1;
			}
		}
		if (n>2|| n<2)	return -1;
		if (flagDeposit ==0){
			
			return -1;
		}
		float LV = stof(sp[1]);
		pList->leverage = LV;
		
		return pList->leverage * pList->deposit;
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}

}
/*---------------------------------Coding By Hung -----------------------------*/

// Instruction Insert
int ProcessData::INS(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	{
				
				return -1;
			}
		}
		if (n>6|| n<6)	{
			
			return -1;
		}
		// Data In
		string BC = sp[1];
		string QC = sp[2];
		int time =  stoi(sp[3]);
		float BP = stof(sp[4]);
		float AP = stof(sp[5]);
		// Init object
		dataFair dataCF = dataFair(BC,QC);
		currFair *newFair = new currFair(dataCF);
		dataEx dataER = dataEx(time,BP,AP);
		exRate *newRate = new exRate(dataER);
		// No have currFair
		if (pList->head == nullptr){
			pList->head = newFair;
			newFair->root = newRate;
			
			return pList->head->root->data.time;
		}
		currFair *preFair = nullptr;
		currFair *nowFair =pList->head;
		while (nowFair != nullptr){
			if (nowFair->data.BC == BC && nowFair->data.QC ==QC){
				bool taller = true;
				nowFair->root = insertRec(nowFair->root,dataER,taller);
				
				return nowFair->root->data.time;
			}
			preFair = nowFair;
			nowFair = nowFair->next;
		}
		// No have currFair satisfy BC and QC 
		if (nowFair == nullptr){
			preFair->next = newFair;
			newFair->root = newRate;
			
			return newFair->root->data.time;
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}

// Instruction Delete
int ProcessData::DEL(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	{
				
				return -1;
			}
		}
		if (n>5|| n<3)	{
			
			return -1;
		}
		if (n==5){
			string BC = sp[1];
			string QC = sp[2];
			int timeA = stoi(sp[3]);
			int timeB = stoi(sp[4]);
			if (timeA > timeB)	{
				
				return -1;
			}
			if (pList->head == nullptr)	{
				
				return -1;
			}
			currFair *nowFair = pList->head;
			while (nowFair != nullptr)
			{
				if (nowFair->data.BC == BC && nowFair->data.QC == QC){
					if (nowFair->root == nullptr)	{
						
						return -1;
					}
					int* sp=new int[1000];
					int count=0;
					remove(sp,count,nowFair->root,timeA,timeB);
					for (int i = 0; i < count; i++)
					{
						bool shorter,success;
						nowFair->removeRec(nowFair->root,sp[i],shorter,success);
					}
					
					return nowFair->root->data.time;
				}
				nowFair = nowFair->next;
			}
		}
		else if (n==4){
			string BC = sp[1];
			string QC = sp[2];
			int time = stoi(sp[3]);
			if (pList->head == nullptr)	{
				
				return -1;
			}
			currFair *nowFair = pList->head;
			while (nowFair != nullptr)
			{
				if (nowFair->data.BC == BC && nowFair->data.QC == QC){
					bool shorter,success;
					nowFair->root = this->removeRec(nowFair->root,time,shorter,success);
					return 	nowFair->root->data.time;
				}
				nowFair = nowFair->next;
			}
		}
		else if (n==3){
			string BC = sp[1];
			string QC = sp[2];
			if (pList->head == nullptr)	return -1;
			currFair *nowFair = pList->head;
			while (nowFair != nullptr)
			{
				if (nowFair->data.BC == BC && nowFair->data.QC == QC){
					clearRec(nowFair->root);
					
					return -1;
				}
				nowFair = nowFair->next;
			}
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}

}

// Instruction Upload
int ProcessData::UPD(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	{
				
				return -1;
			}
		}
		 if (n>6||n<6)	{
			 
			 return -1;
		 }
		// Data in
		string BC = sp[1];
		string QC = sp[2];
		int time = stoi(sp[3]);
		float BP = stof(sp[4]);
		float AP = stof(sp[5]);
		if (BP >AP)	{
			
			return -1;
		}
		if (pList->head == nullptr)	{
			
			return -1;
		}
		currFair *nowFair = pList->head;
		while (nowFair != nullptr)
		{
			if (nowFair->data.BC == BC && nowFair->data.QC == QC){
				int result =0;
				upLoad(nowFair->root,time,BP,AP,result);
				if (result ==1)	{
					
					return nowFair->root->data.time;
				}
				if (result==0)	{
					
					return -1;
				}	
			}
			nowFair = nowFair->next;
		}
		return  -1;

	}
	catch(const std::exception& e)
	{
		return -1;
	}

}

// Instruction OB
int ProcessData::OB(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	{
				
				return -1;
			}
		}
		 if (n>6|| n<6)	{
			 
			 return -1;
		 }
		string BC = sp[1];
		string QC = sp[2];
		if (BC != "USD" && QC !="USD")	{
			
			return -1;
		}
		int time = stoi(sp[3]);
		float LOT = stof(sp[4]);
		int ID = stoi(sp[5]);
		// Kiểm tra trùng nhau
		if (pList->size ==0){
			pList->arrID[pList->size]=ID;
			pList->size=1;
		}
		else
		{
			for (int i = 0; i < pList->size; i++)
			{
				if (ID == pList->arrID[i])	{
					
					return -1;
				}
			}
			pList->size+=1;
			pList->arrID[pList->size-1] = ID;
		}
		if (pList->head == nullptr)	{
			
			return -1;
		}
		currFair *nowFair = pList->head;
		while (nowFair != nullptr)
		{
			if 	(nowFair->data.BC == BC && nowFair->data.QC == QC){
				exRate *save = new exRate();
				findNode(nowFair->root,time,save);
				float buyCost =save->data.AP;
				float sellCost=save->data.BP;
				int BorS =0;
				nodeID *newID = new nodeID(ID,buyCost,LOT,BorS,BC,QC);
				if (pID->phead == nullptr){
					pID->phead = newID;
					pID->count++;
				}
				else{
					nodeID *nowNode = pID->phead;
					while (nowNode->next != nullptr)
					{
						nowNode = nowNode->next;
					}
					nowNode->next = newID;
					pID->count++;
				}
				if (QC == "USD"){
					float result = buyCost*LOT*100000;
					// Khối lượng giao dịch lớn hơn số tiền có thể giao dịch
					if (result > pList->deposit*pList->leverage){
						pList->arrID[pList->size-1] = 0;
						pList->size -=1;
						
						return -1;
					}
					
					return ceil(result);
				}
				else if (BC == "USD"){
					float result = (buyCost*LOT*100000)/sellCost;
					if (result > pList->deposit*pList->leverage){
						pList->arrID[pList->size-1] = 0;
						pList->size -=1;
						
						return -1;
					}
					
					return ceil(result);
				}		
			}
			nowFair = nowFair->next;
		}
		return -1;

	}
	catch(const std::exception& e)
	{
		return -1;
	}
}

// Instruction OS
int ProcessData::OS(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	{
				
				return -1;
			}
		}
		if (n>6||n<6)	{
			
			return -1;
		}
		string BC = sp[1];
		string QC = sp[2];
		if (BC != "USD" && QC !="USD")	{
			
			return -1;
		}
		int time = stoi(sp[3]);
		float LOT = stof(sp[4]);
		int ID = stoi(sp[5]);
		// Kiểm tra các ID trùng nhau
		if (pList->size ==0){
			pList->arrID[pList->size]=ID;
			pList->size=1;
		}
		else
		{
			for (int i = 0; i < pList->size; i++)
			{
				if (ID == pList->arrID[i])	{
					
					return -1;
				}
			}
			pList->size+=1;
			pList->arrID[pList->size-1] = ID;
		}
		if (pList->head == nullptr)	{
			
			return -1;
		}
		currFair *nowFair = pList->head;
		while (nowFair != nullptr)
		{
			if 	(nowFair->data.BC == BC && nowFair->data.QC == QC){
				exRate *save = new exRate();
				findNode(nowFair->root,time,save);
				float sellCost =save->data.BP;
				float buyCost =save->data.AP;
				int BorS=1;
				nodeID *newID = new nodeID(ID,sellCost,LOT,BorS,BC,QC);
					if (pID->phead == nullptr){
						pID->phead = newID;
						pID->count++;
					}
					else{
						nodeID *nowNode = pID->phead;
						while (nowNode->next != nullptr)
						{
							nowNode = nowNode->next;
						}
						nowNode->next = newID;
						pID->count++;
					}
					if (QC == "USD"){
						float result = sellCost*LOT*100000;
						// Khối lượng giao dịch lớn hơn số tiền có thể giao dịch
						if (result >pList->deposit*pList->leverage){
							pList->arrID[pList->size-1] = 0;
							pList->size -=1;
							
							return -1;
						}
						
						return ceil(result);
					}
					else if (BC == "USD"){
						float result = (sellCost*LOT*100000)/buyCost;
						if (result > pList->deposit*pList->leverage){	
							pList->arrID[pList->size-1] = 0;
							pList->size -=1;
							
							return -1;
						}
						
						return ceil(result);
					}
				
			}
			nowFair = nowFair->next;
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}
// Instruction CB
int ProcessData::CB(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	{
				
				return -1;
			}
		}
		if (n>3||n<3)	{
			
			return -1;
		}
		// Data In
		int time = stoi(sp[1]);
		int ID = stoi(sp[2]);
		int flag =0;
		// Kiểm tra ID đã có hay chưa
		for (int i=0;i<pList->size;i++){
			if (pList->arrID[i]== ID){
				flag =1;
				pList->arrID[i] = -27;
				break;
			}
		}
		if (flag ==0)	{
			
			return -1;
		}
		if (pID->phead == nullptr)	{
			
			return -1;
		}
		nodeID *nowID = pID->phead;
		while (nowID !=nullptr)
		{
			if (nowID->ID == ID && nowID->BorS == 1 ){
				string BC = nowID->BC;
				string QC = nowID->QC;
				currFair *nowFair = pList->head;
				while (nowFair != nullptr)
				{
					if (nowFair->data.BC == BC && nowFair->data.QC == QC){
						exRate *save = new exRate();
						findNode(nowFair->root,time,save);
						if (QC == "USD"){
							float moneyBuy = save->data.AP * nowID->scale* 100000;
							float moneySell = nowID->cost * nowID->scale*100000;
							float profit = moneySell - moneyBuy;
							pList->deposit += profit;
							
							return ceil(profit);
						}
						else if (BC == "USD"){
							float moneyBuy = nowID->scale* 100000;
							float moneySell =  (nowID->cost * nowID->scale*100000)/ save->data.AP;
							float profit =moneySell-moneyBuy;
							pList->deposit += profit;
							
							return ceil(profit);
						}
					}
					nowFair = nowFair->next;
				}
				
			}
			nowID = nowID->next;
		}
		return -1;
		
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}

// Instruction CS
int ProcessData::CS(const string *sp, const int n)
{
	try
	{
		for (int i = 0; i < n; i++)
		{
			if (sp[i] == "")	{
				
				return -1;
			}
		}
		 if (n>3||n<3)	{
			 
			 return -1;
		 }
		// Data In
		int time = stoi(sp[1]);
		int ID = stoi(sp[2]);
		int flag1 =0;
		// Kiểm tra ID đã có hay chưa
		for (int i=0;i<pList->size;i++){
			if (ID== pList->arrID[i]){
				flag1 =1;
				pList->arrID[i] = -27;
				break;
			}
		}
		if (flag1 ==0)	{
			
			return -1;
		}
		if (pID->phead == nullptr)	{
			
			return -1;
		}
		nodeID *nowID = pID->phead;
		while (nowID !=nullptr)
		{
			if (nowID->ID == ID && nowID->BorS == 0 ){
				string BC = nowID->BC;
				string QC = nowID->QC;
				currFair *nowFair = pList->head;
				while (nowFair != nullptr)
				{
					if (nowFair->data.BC == BC && nowFair->data.QC == QC){
						exRate *save = new exRate();
						findNode(nowFair->root,time,save);
						if (QC == "USD"){
							float moneySell = save->data.BP *nowID->scale*100000;
							float moneyBuy = nowID->cost * nowID->scale * 100000;
							float profit = moneySell-moneyBuy;
							pList->deposit += profit;
							
							return ceil(profit);
						}
						else if (BC == "USD"){
							float moneySell = (save->data.BP * nowID->scale*100000)/save->data.AP;
							float moneyBuy = nowID->scale*100000;
							float profit = moneySell - moneyBuy;
							pList->deposit += profit;
							
							return ceil(profit);
						}
					}
					nowFair = nowFair->next;
				}
				
			}
			nowID = nowID->next;
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		return -1;
	}
}

int main(int argc, char** argv) {
    ProcessData * data = new ProcessData();
	
    string line;
    while (getline(cin, line)) { 
    	cout  << data->process(line) << endl;
    }
    delete data;

	return EXIT_SUCCESS;
}
