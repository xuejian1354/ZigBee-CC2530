#include "OSAL.h"
#include "list.h"
//��������ͷ���data=0;pNext=null

Node *head ;
bool createNodelist(void)
{
	head = (Node*)osal_mem_alloc(sizeof(Node));
	if(NULL == head){
		return list_false;
		}
	else {
		head->short_dev =0;
                head->addr_dev =0;
		head->pNext = NULL;
		return list_true;
		}
}

//���ӽڵ�β�����
bool  addNode(Node * node)
{
	if(NULL == head){
		return list_false;
		}
	Node* p = head ->pNext;
	Node* q = head;
	while(NULL !=p){
		q = p;
		p = p->pNext;
		}
	q->pNext = node;
	node ->pNext = NULL;	
	return list_true;
}

//ɾ���ڵ�
bool deleteNode(uint8 index)
{
	if(NULL == head){
		return list_false;
		}
	Node* perior = head;
	Node* q = head->pNext;
	while((q!=NULL)&&(q->addr_dev != index))
		{
		perior = q;
		q = q->pNext;
		}
	if(q->addr_dev == index)
		{
		if(q->pNext == NULL)
			{

			perior->pNext = NULL;
		        osal_mem_free(q);
			return list_true;
			}
		else
			{
			perior->pNext = q->pNext;
    			osal_mem_free(q);
			return list_true;
			}
		}
        return list_false;
}

//����
void destroyNodeList(void)
{
	if(NULL == head){
		return;
		}
	if(NULL == head->pNext){
		osal_mem_free(head);
		head =NULL;
		return;
		}
	Node* p = head->pNext;
	while(NULL != p){
		Node* tmp =p;
		p = p->pNext;
		osal_mem_free(tmp);
		}
	osal_mem_free(head);
	head = NULL;
}

void setNodeList(Node * node)
{
  Node* perior = head;
  uint8 count=0;
  while(NULL != perior)
  {
    if(perior->addr_dev==node->addr_dev)//ͨ���豸��ַȷ���Ƿ���list����
    {
      count=1;
      perior->short_dev=node->short_dev;//if in the list,cover the short_dev
      break;
    }
    else
    count=0;
    perior=perior->pNext;
  }
  if(count==0)
  {
     addNode(node);
  }
}

uint16 get_NodeList(uint8 dev)
{
   Node* p = head;
  while(NULL != p)
  {
    if(p->addr_dev==dev)
    {
      return p->short_dev;
    }
    else
    p=p->pNext;
  }
  return NULL;
}
