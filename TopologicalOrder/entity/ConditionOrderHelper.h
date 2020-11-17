#pragma once
#include <vector>
#include <memory>
#include <set>
#include <map>
using namespace std;

/*
*    ��֪���ϵ�нڵ��˳���Լ����ɸ���ͬϵ�м�Ľڵ�֮ǰ���Ⱥ�˳�����󣬼���õ����ϵ�����������˳��
*   ��ʹ��˫������������ͼ��
* */

class LinkListNode {
public:
	int id = 0;
	vector<shared_ptr<LinkListNode>>  preNodes;      //ǰ�ڵ�
	vector<shared_ptr<LinkListNode>>  postNodes;     //��ڵ�
};

//����˳�����
class ConditionOrderHelper
{
public:
	//����
	void Reset();

	//׷�ӹ���һ���µ�˳��
	bool AppendNewOrders(vector<int> ids);

	//Ӧ��˳������
	bool ApplyOrderCondition(pair<int, int> order);
	bool ApplyOrderCondition(int idPre, int idPost);

	//���������յ�˳��
	vector<int> CalculateFinalOrder();
private:
	//ȷ��ͼ�Ľڵ����ֻ��һ��ǰ�ڵ�
	void EnsureGraphSinglePreNode(shared_ptr<LinkListNode> node);

	//�Ƿ��ܹ����ʵ� id
	bool CanReachNodeWithId(shared_ptr<LinkListNode> node, int id);

	//�ݹ��ռ�ӵ�ж��ǰ�ڵ�Ľڵ㼯��
	void CollectMultiPreNodeRecursively(shared_ptr<LinkListNode> node);

	//���Ӧ������֮���Ƿ��γ�ѭ��
	bool CheckLoopAfterCondition(int idPre, int idPost);

	//����Ƿ���ڱջ�
	bool CheckLoop(shared_ptr<LinkListNode> node);
	bool CheckLoopRecursively(shared_ptr<LinkListNode> node); //ע������ CheckLoop ����

	//�����õ�����ͼ�����нڵ� id
	vector<int> TravelGraph(shared_ptr<LinkListNode> node);
	vector<int> TravelGraphRecursively(shared_ptr<LinkListNode> node); //ע������ TravelGraph ����

	//����һ������ͼ
	shared_ptr<LinkListNode> CopyGraph(shared_ptr<LinkListNode> graph);

	//�ڵ���CopyGraph ǰ��Ҫ�ȵ���
	void InitCopyGraph();

private:
	set<int> idsHasTravel;                                        //�Ѿ������˵Ľڵ㣬���� TravelGraph 
	set<shared_ptr<LinkListNode>>       nodesHasMuiltiPreNode;    //���ڶ��ǰ�ڵ�Ľڵ㣬���� EnsureGraphSinglePreNode ����ʹ��
	vector<int> idStack;                                          //idջ�����ڼ���Ƿ���ڱջ�
	map<int, shared_ptr<LinkListNode>>  nodesPool;                //�ڵ�أ������ҵ�������ǰ��2���ڵ㣩
	map<int, shared_ptr<LinkListNode>>  nodesPoolForCopy;         //�ڵ�أ���ֹ�ظ������µ�����ָ��ڵ㣩
	shared_ptr<LinkListNode> root = make_shared<LinkListNode>();  //��Դ�ڵ�
};