#include "ConditionOrderHelper.h"

//����
void ConditionOrderHelper::Reset()
{
	root->id = 0;
	root->postNodes.clear();
	root->preNodes.clear();
	nodesPool.clear();
}

//׷�ӹ���һ���µ�˳��
bool ConditionOrderHelper::AppendNewOrders(vector<int> ids)
{
	//�ڲ�ʵ������ִ�е����ں���
	vector<int> idsTmp(ids.rbegin(), ids.rend());
	ids = idsTmp;

	if (ids.empty())
		return true;

	//���ids �е�ÿһ��id �Ƿ��Ѿ����ڣ������ڣ��򷵻� false ʧ��
	for (auto id : ids)
	{
		if (nodesPool.count(id))
			return false;
	}

	auto current = root;
	//��ӵ�һ���ڵ�
	{
		auto node = make_shared<LinkListNode>();
		node->id = ids.front();
		node->preNodes.push_back(current);   //���ǰ�ڵ�
		nodesPool.emplace(ids.front(), node);

		current->postNodes.insert(current->postNodes.begin(), node); //�����ⲿ���������ں��棬���ڲ�����˳����������������Ժ����ļ��� postNodes ǰ��
		current = node;
	}

	//���� ids ��ǰ���󴴽�һ������ͼ
	for (size_t i = 1; i < ids.size(); ++i)
	{
		auto id = ids[i];

		auto node = make_shared<LinkListNode>();
		node->id = id;
		node->preNodes.push_back(current);   //���ǰ�ڵ�
		nodesPool.emplace(id, node);         

		current->postNodes.push_back(node);  //ǰ�ڵ������ node Ϊ��ڵ�
		current = node;
	}

	return true;
}

//Ӧ��˳������
bool ConditionOrderHelper::ApplyOrderCondition(pair<int, int> order)
{
	return ApplyOrderCondition(order.first, order.second);
}

bool ConditionOrderHelper::ApplyOrderCondition(int idPre, int idPost)
{
	//�ڲ�ʵ������ִ�е����ں���
	swap(idPre, idPost);

	if (idPre == idPost)
		return false;

	//idPre �� idPost ���������
	if (!nodesPool.count(idPre))
		return false;
	if (!nodesPool.count(idPost))
		return false;

	////�����γɱջ�
	//if (CheckLoopAfterCondition(idPre, idPost))
	//	return false;

	//����ǰ������
	auto& pre = nodesPool[idPre];
	auto& post = nodesPool[idPost];
	pre->postNodes.push_back(post);
	post->preNodes.push_back(pre);

	//����Ƿ��γɱջ�
	bool loop = CheckLoop(root);
	if (loop)
	{
		//���ڱջ�������ӣ�����ʧ��
		pre->postNodes.pop_back();
		post->preNodes.pop_back();
		return false;
	}
	
	return true;
}

//���������յ�˳��
vector<int> ConditionOrderHelper::CalculateFinalOrder()
{
	InitCopyGraph();
	auto graph = CopyGraph(root);

	EnsureGraphSinglePreNode(graph);

	auto orders = TravelGraph(graph);

	orders.erase(orders.begin());

	//�ڲ�ʵ������ִ�е����ں���(����ʱ�����µ���˳��)
	vector<int> ordersTmp(orders.rbegin(), orders.rend());
	orders = ordersTmp;

	return orders;
}

//ȷ��ͼ�Ľڵ����ֻ��һ��ǰ�ڵ�
void ConditionOrderHelper::EnsureGraphSinglePreNode(shared_ptr<LinkListNode> node)
{
	//�ռ�����2������ǰ�ڵ�Ľڵ�
	nodesHasMuiltiPreNode.clear();
	CollectMultiPreNodeRecursively(node);

	//�Ͽ���������У��϶̵����ӣ�һ��ʣ��һ����
	for (auto n : nodesHasMuiltiPreNode)
	{
		//�����ǰ�ڵ�֮�䣨A,B...����A �ܹ����� B, ��ȥ�� A �� n ������
		auto parents = n->preNodes;

		set<int> idPreToDelete;
		for (size_t i = 0; i < parents.size(); ++i)
		{
			for (size_t j = i + 1; j < parents.size(); ++j)
			{
				auto n1 = parents[i];
				auto n2 = parents[j];
				if (CanReachNodeWithId(n1, n2->id))
					idPreToDelete.insert(n1->id);
				else if(CanReachNodeWithId(n2, n1->id))
					idPreToDelete.insert(n2->id);
			}
		}

		if (!idPreToDelete.empty())
		{
			for (auto iter = n->preNodes.begin(); iter != n->preNodes.end(); )
			{
				auto& pn = *iter;
				if (idPreToDelete.count(pn->id))
				{
					//ɾ��ǰ�ڵ���ָ�� n �Ľڵ�
					for (auto iter2 = pn->postNodes.begin(); iter2 != pn->postNodes.end();)
					{
						if ((*iter2)->id == n->id)
							iter2 = pn->postNodes.erase(iter2);
						else
							++iter2;
					}

					//ɾ�� n ָ��ǰ�汻ɾ���Ľڵ�
					iter = n->preNodes.erase(iter);
				}
				else
					++iter;
			}
		}
	}
}

//�Ƿ��ܹ����ʵ� id
bool ConditionOrderHelper::CanReachNodeWithId(shared_ptr<LinkListNode> node, int id)
{
	if (node->id == id)
		return true;

	for (auto n : node->postNodes)
	{
		if (CanReachNodeWithId(n, id))
			return true;
	}

	return false;
}

//�ݹ��ռ�ӵ�ж��ǰ�ڵ�Ľڵ㼯��
void ConditionOrderHelper::CollectMultiPreNodeRecursively(shared_ptr<LinkListNode> node)
{
	if (node->preNodes.size() >= 2)
		nodesHasMuiltiPreNode.insert(node);

	for (auto n : node->postNodes)
	{
		CollectMultiPreNodeRecursively(n);
	}
}

//���Ӧ������֮���Ƿ��γ�ѭ��
bool ConditionOrderHelper::CheckLoopAfterCondition(int idPre, int idPost)
{
	InitCopyGraph();
	auto graph = CopyGraph(root);

	//idPre �� idPost ���������
	if (!nodesPoolForCopy.count(idPre))
		return false;
	if (!nodesPoolForCopy.count(idPost))
		return false;

	//����ǰ������
	auto& pre = nodesPool[idPre];
	auto& post = nodesPool[idPost];
	pre->postNodes.push_back(post);
	post->preNodes.push_back(pre);

	bool loop = CheckLoop(graph);
	return loop;
}

//����Ƿ���ڱջ�
bool ConditionOrderHelper::CheckLoop(shared_ptr<LinkListNode> node)
{
	idStack.clear();
	return CheckLoopRecursively(node);
}
bool ConditionOrderHelper::CheckLoopRecursively(shared_ptr<LinkListNode> node)
{
	//���ջ�д������е� id ֱ��˵�����ڱջ�
	for (auto& id : idStack)
	{
		if (id == node->id)
			return true;
	}

	//����ǰ�ڵ����ջ��
	idStack.push_back(node->id);

	for (auto post : node->postNodes)
	{
		if (CheckLoopRecursively(post))
			return true;
	}

	idStack.pop_back();
	return false;
}


//�����õ�����ͼ�����нڵ� id
vector<int> ConditionOrderHelper::TravelGraph(shared_ptr<LinkListNode> node)
{
	InitCopyGraph();
	auto graph = CopyGraph(node);

	idsHasTravel.clear();
	return TravelGraphRecursively(graph);
}
vector<int> ConditionOrderHelper::TravelGraphRecursively(shared_ptr<LinkListNode> node)
{
	vector<int> ids;
	for (auto iter = node->postNodes.rbegin(); iter != node->postNodes.rend(); ++iter)
	{
		if (idsHasTravel.count((*iter)->id) == 0) //û�ռ����������
		{
			//�ݹ��ռ��ӽڵ�� id
			vector<int> idsOfPost = TravelGraphRecursively(*iter);
			ids.insert(ids.begin(), idsOfPost.begin(), idsOfPost.end());
		}
	}

	node->postNodes.clear(); //�ռ���ϣ�����ӽڵ�(�����ӽڵ������е��丸�ڵ�֮�����ϵ)

	//���Լ��ռ�����
	ids.insert(ids.begin(), node->id);
	idsHasTravel.insert(node->id);

	////�Լ��ռ���Ϻ���Ҫ������ǰ�ڵ��У�ɾ���Լ�
	//for (auto pre : node->preNodes)
	//{
	//	bool bFound = false;
	//	for (auto iter = pre->postNodes.begin(); iter != pre->postNodes.end(); ++iter)
	//	{
	//		if ((*iter)->id == node->id)
	//		{
	//			pre->postNodes.erase(iter);
	//			break;
	//		}
	//	}

	//}

	return ids;
}

//����һ������ͼ
shared_ptr<LinkListNode> ConditionOrderHelper::CopyGraph(shared_ptr<LinkListNode> graph)
{
	//ȡ�û�ô����½ڵ�
	shared_ptr<LinkListNode> graphNew;
	if (nodesPoolForCopy.count(graph->id))
		graphNew = nodesPoolForCopy[graph->id];
	else
	{
		graphNew = make_shared<LinkListNode>();
		graphNew->id = graph->id;
		nodesPoolForCopy.emplace(graphNew->id, graphNew);

		//�����ӽڵ�
		for (auto& node : graph->postNodes)
		{
			auto nodeNew = CopyGraph(node);
			graphNew->postNodes.push_back(nodeNew);
		}
	}

	//��ʼ���ӽڵ�ĸ��ڵ��ֵ
	for (auto& node : graphNew->postNodes)
	{
		bool bFound = false;
		for (auto& pre: node->preNodes)
		{
			if (pre->id == graphNew->id)
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
			node->preNodes.push_back(graphNew);
	}

	//���ظ��Ƶõ���ͼ
	return graphNew;
}

//�ڵ���CopyGraph ǰ��Ҫ�ȵ���
void ConditionOrderHelper::InitCopyGraph()
{
	nodesPoolForCopy.clear();
}
