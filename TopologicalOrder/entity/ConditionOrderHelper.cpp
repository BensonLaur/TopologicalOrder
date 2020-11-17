#include "ConditionOrderHelper.h"

//重置
void ConditionOrderHelper::Reset()
{
	root->id = 0;
	root->postNodes.clear();
	root->preNodes.clear();
	nodesPool.clear();
}

//追加构建一个新的顺序
bool ConditionOrderHelper::AppendNewOrders(vector<int> ids)
{
	//内部实现是先执行的排在后面
	vector<int> idsTmp(ids.rbegin(), ids.rend());
	ids = idsTmp;

	if (ids.empty())
		return true;

	//检测ids 中的每一个id 是否已经存在，若存在，则返回 false 失败
	for (auto id : ids)
	{
		if (nodesPool.count(id))
			return false;
	}

	auto current = root;
	//添加第一个节点
	{
		auto node = make_shared<LinkListNode>();
		node->id = ids.front();
		node->preNodes.push_back(current);   //添加前节点
		nodesPool.emplace(ids.front(), node);

		current->postNodes.insert(current->postNodes.begin(), node); //由于外部后插入的排在后面，而内部遍历顺序是右序遍历，所以后加入的加在 postNodes 前面
		current = node;
	}

	//遍历 ids 从前到后创建一个单链图
	for (size_t i = 1; i < ids.size(); ++i)
	{
		auto id = ids[i];

		auto node = make_shared<LinkListNode>();
		node->id = id;
		node->preNodes.push_back(current);   //添加前节点
		nodesPool.emplace(id, node);         

		current->postNodes.push_back(node);  //前节点中添加 node 为后节点
		current = node;
	}

	return true;
}

//应用顺序条件
bool ConditionOrderHelper::ApplyOrderCondition(pair<int, int> order)
{
	return ApplyOrderCondition(order.first, order.second);
}

bool ConditionOrderHelper::ApplyOrderCondition(int idPre, int idPost)
{
	//内部实现是先执行的排在后面
	swap(idPre, idPost);

	if (idPre == idPost)
		return false;

	//idPre 和 idPost 都必须存在
	if (!nodesPool.count(idPre))
		return false;
	if (!nodesPool.count(idPost))
		return false;

	////不能形成闭环
	//if (CheckLoopAfterCondition(idPre, idPost))
	//	return false;

	//建立前后连接
	auto& pre = nodesPool[idPre];
	auto& post = nodesPool[idPost];
	pre->postNodes.push_back(post);
	post->preNodes.push_back(pre);

	//检测是否形成闭环
	bool loop = CheckLoop(root);
	if (loop)
	{
		//存在闭环则撤销添加，返回失败
		pre->postNodes.pop_back();
		post->preNodes.pop_back();
		return false;
	}
	
	return true;
}

//计算获得最终的顺序
vector<int> ConditionOrderHelper::CalculateFinalOrder()
{
	InitCopyGraph();
	auto graph = CopyGraph(root);

	EnsureGraphSinglePreNode(graph);

	auto orders = TravelGraph(graph);

	orders.erase(orders.begin());

	//内部实现是先执行的排在后面(返回时，重新调整顺序)
	vector<int> ordersTmp(orders.rbegin(), orders.rend());
	orders = ordersTmp;

	return orders;
}

//确保图的节点最多只有一个前节点
void ConditionOrderHelper::EnsureGraphSinglePreNode(shared_ptr<LinkListNode> node)
{
	//收集含有2个以上前节点的节点
	nodesHasMuiltiPreNode.clear();
	CollectMultiPreNodeRecursively(node);

	//断开多段连接中，较短的连接（一般剩下一条）
	for (auto n : nodesHasMuiltiPreNode)
	{
		//如果在前节点之间（A,B...），A 能够到达 B, 则去掉 A 到 n 的连接
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
					//删除前节点中指向 n 的节点
					for (auto iter2 = pn->postNodes.begin(); iter2 != pn->postNodes.end();)
					{
						if ((*iter2)->id == n->id)
							iter2 = pn->postNodes.erase(iter2);
						else
							++iter2;
					}

					//删除 n 指向前面被删除的节点
					iter = n->preNodes.erase(iter);
				}
				else
					++iter;
			}
		}
	}
}

//是否能够访问到 id
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

//递归收集拥有多个前节点的节点集合
void ConditionOrderHelper::CollectMultiPreNodeRecursively(shared_ptr<LinkListNode> node)
{
	if (node->preNodes.size() >= 2)
		nodesHasMuiltiPreNode.insert(node);

	for (auto n : node->postNodes)
	{
		CollectMultiPreNodeRecursively(n);
	}
}

//检查应用条件之后是否形成循环
bool ConditionOrderHelper::CheckLoopAfterCondition(int idPre, int idPost)
{
	InitCopyGraph();
	auto graph = CopyGraph(root);

	//idPre 和 idPost 都必须存在
	if (!nodesPoolForCopy.count(idPre))
		return false;
	if (!nodesPoolForCopy.count(idPost))
		return false;

	//建立前后连接
	auto& pre = nodesPool[idPre];
	auto& post = nodesPool[idPost];
	pre->postNodes.push_back(post);
	post->preNodes.push_back(pre);

	bool loop = CheckLoop(graph);
	return loop;
}

//检测是否存在闭环
bool ConditionOrderHelper::CheckLoop(shared_ptr<LinkListNode> node)
{
	idStack.clear();
	return CheckLoopRecursively(node);
}
bool ConditionOrderHelper::CheckLoopRecursively(shared_ptr<LinkListNode> node)
{
	//如果栈中存在已有的 id 直接说明存在闭环
	for (auto& id : idStack)
	{
		if (id == node->id)
			return true;
	}

	//将当前节点加入栈中
	idStack.push_back(node->id);

	for (auto post : node->postNodes)
	{
		if (CheckLoopRecursively(post))
			return true;
	}

	idStack.pop_back();
	return false;
}


//遍历得到有向图的所有节点 id
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
		if (idsHasTravel.count((*iter)->id) == 0) //没收集过的情况下
		{
			//递归收集子节点的 id
			vector<int> idsOfPost = TravelGraphRecursively(*iter);
			ids.insert(ids.begin(), idsOfPost.begin(), idsOfPost.end());
		}
	}

	node->postNodes.clear(); //收集完毕，清除子节点(包括子节点与所有的其父节点之间的联系)

	//将自己收集加入
	ids.insert(ids.begin(), node->id);
	idsHasTravel.insert(node->id);

	////自己收集完毕后，需要从所有前节点中，删除自己
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

//复制一个有向图
shared_ptr<LinkListNode> ConditionOrderHelper::CopyGraph(shared_ptr<LinkListNode> graph)
{
	//取得获得创建新节点
	shared_ptr<LinkListNode> graphNew;
	if (nodesPoolForCopy.count(graph->id))
		graphNew = nodesPoolForCopy[graph->id];
	else
	{
		graphNew = make_shared<LinkListNode>();
		graphNew->id = graph->id;
		nodesPoolForCopy.emplace(graphNew->id, graphNew);

		//复制子节点
		for (auto& node : graph->postNodes)
		{
			auto nodeNew = CopyGraph(node);
			graphNew->postNodes.push_back(nodeNew);
		}
	}

	//初始化子节点的父节点的值
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

	//返回复制得到的图
	return graphNew;
}

//在调用CopyGraph 前需要先调用
void ConditionOrderHelper::InitCopyGraph()
{
	nodesPoolForCopy.clear();
}
