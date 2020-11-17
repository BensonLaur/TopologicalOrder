#pragma once
#include <vector>
#include <memory>
#include <set>
#include <map>
using namespace std;

/*
*    已知多个系列节点的顺序，以及若干个不同系列间的节点之前的先后顺序需求，计算得到多个系列整体的最终顺序
*   （使用双向链表构建有向图）
* */

class LinkListNode {
public:
	int id = 0;
	vector<shared_ptr<LinkListNode>>  preNodes;      //前节点
	vector<shared_ptr<LinkListNode>>  postNodes;     //后节点
};

//条件顺序计算
class ConditionOrderHelper
{
public:
	//重置
	void Reset();

	//追加构建一个新的顺序
	bool AppendNewOrders(vector<int> ids);

	//应用顺序条件
	bool ApplyOrderCondition(pair<int, int> order);
	bool ApplyOrderCondition(int idPre, int idPost);

	//计算获得最终的顺序
	vector<int> CalculateFinalOrder();
private:
	//确保图的节点最多只有一个前节点
	void EnsureGraphSinglePreNode(shared_ptr<LinkListNode> node);

	//是否能够访问到 id
	bool CanReachNodeWithId(shared_ptr<LinkListNode> node, int id);

	//递归收集拥有多个前节点的节点集合
	void CollectMultiPreNodeRecursively(shared_ptr<LinkListNode> node);

	//检查应用条件之后是否形成循环
	bool CheckLoopAfterCondition(int idPre, int idPost);

	//检测是否存在闭环
	bool CheckLoop(shared_ptr<LinkListNode> node);
	bool CheckLoopRecursively(shared_ptr<LinkListNode> node); //注：仅由 CheckLoop 调用

	//遍历得到有向图的所有节点 id
	vector<int> TravelGraph(shared_ptr<LinkListNode> node);
	vector<int> TravelGraphRecursively(shared_ptr<LinkListNode> node); //注：仅由 TravelGraph 调用

	//复制一个有向图
	shared_ptr<LinkListNode> CopyGraph(shared_ptr<LinkListNode> graph);

	//在调用CopyGraph 前需要先调用
	void InitCopyGraph();

private:
	set<int> idsHasTravel;                                        //已经遍历了的节点，用于 TravelGraph 
	set<shared_ptr<LinkListNode>>       nodesHasMuiltiPreNode;    //存在多个前节点的节点，用于 EnsureGraphSinglePreNode 计算使用
	vector<int> idStack;                                          //id栈，用于检测是否存在闭环
	map<int, shared_ptr<LinkListNode>>  nodesPool;                //节点池（用于找到条件的前后2个节点）
	map<int, shared_ptr<LinkListNode>>  nodesPoolForCopy;         //节点池（防止重复创建新的智能指针节点）
	shared_ptr<LinkListNode> root = make_shared<LinkListNode>();  //根源节点
};