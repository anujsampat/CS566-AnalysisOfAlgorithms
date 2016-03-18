/*
 * Anuj Sampat
 * CS566 Programming Assignment 2: Binary Search Tree Operations
 */

#include <stdio.h>
#include <stdlib.h>

#define INSERT		1
#define DELETE		2
#define TRAVERSE	3
#define EXIT		4

char user_input[100];

typedef struct node_t
{
  int key;
  struct node_t *parent;
  struct node_t *left;
  struct node_t *right;
} NODE;

NODE *root = NULL;

static NODE * get_root (NODE * tree)
{
  return root;
}

static void set_root (NODE * node)
{
  root = node;
}

static NODE * TreeSearch (NODE * x, int k)
{
  if ((x == NULL) || (x->key == k))
    return x;

  if (k < x->key)
    return TreeSearch (x->left, k);
  else
    return TreeSearch (x->right, k);
}

static void SubTreeTransplant (NODE * T, NODE * u, NODE * v)
{
  if (u->parent == NULL)
    set_root (v);
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  if (v != NULL)
    v->parent = u->parent;
}

static NODE * TreeMinimum (NODE * x)
{
  while (x->left != NULL)
    x = x->left;

  return x;
}

static void TreeInsert (NODE * tree, NODE * z)
{
  NODE *y = NULL;
  NODE *x = get_root (tree);

  while (x != NULL)
    {
      y = x;
      if (z->key < x->key)
	x = x->left;
      else
	x = x->right;
    }

  z->parent = y;
  if (y == NULL)
    set_root (z);
  else if (z->key < y->key)
    y->left = z;
  else
    y->right = z;
}

static void TreeDelete (NODE * T, NODE * z)
{
  NODE *y;

  if (z->left == NULL)
    SubTreeTransplant (T, z, z->right);
  else if (z->right == NULL)
    SubTreeTransplant (T, z, z->left);
  else
    {
      y = TreeMinimum (z->right);
      if (y->parent != z)
	{
	  SubTreeTransplant (T, y, y->right);
	  y->right = z->right;
	  y->right->parent = y;
	}

      SubTreeTransplant (T, z, y);
      y->left = z->left;
      y->left->parent = y;
    }
}

static void PostOrderTreeTraverse (NODE * node)
{
  if (node != NULL)
    {
      PostOrderTreeTraverse (node->left);
      PostOrderTreeTraverse (node->right);
      printf ("%d ", node->key);
    }
}

static void main_menu ()
{
  printf ("\nMAIN MENU\n\n");
  printf ("Insert: %d\n", INSERT);
  printf ("Delete: %d\n", DELETE);
  printf ("Post Order Tree Traversal: %d\n", TRAVERSE);
  printf ("Exit: %d\n", EXIT);
}

static int get_user_input ()
{
  int ret_val;

  fgets (user_input, sizeof (user_input), stdin);
  sscanf (user_input, "%d", &ret_val);

  return ret_val;
}

int main (void)
{
  int user_cmd = 0;
  int input_key;
  NODE *node_op = NULL;

  main_menu ();

  while (user_cmd != EXIT)
    {
      printf
	("\nEnter the operation number for the operation you want to perform and hit Enter:");
      user_cmd = get_user_input ();

      switch (user_cmd)
	{
	case 1:
	  printf ("Type a key to be inserted and hit Enter:");
	  input_key = get_user_input ();

	  node_op = (NODE *) malloc (sizeof (NODE));
	  if (node_op != NULL)
	    {
	      node_op->key = input_key;
	      node_op->parent = NULL;
	      node_op->left = NULL;
	      node_op->right = NULL;
	      TreeInsert (root, node_op);
	    }
	  else
	    printf ("Could not insert key!\n");

	  main_menu ();
	  break;

	case 2:
	  printf ("Type a key to be deleted and hit Enter:");
	  input_key = get_user_input ();

	  node_op = TreeSearch (root, input_key);

	  if (node_op == NULL)
	    printf ("\nKey not in tree\n");
	  else
	    TreeDelete (root, node_op);

	  free (node_op);	// free the memory

	  main_menu ();
	  break;

	case 3:
	  if (root != NULL)
	    {
	      printf ("\n");
	      PostOrderTreeTraverse (root);
	      printf ("\n");
	    }
	  else
	    printf ("\nTree is empty\n");
	  printf ("\n\nHit Enter to return to the MAIN MENU");
	  input_key = get_user_input ();

	  main_menu ();
	  break;

	case 4:
	default:
	  break;
	}
    }

  return 0;
}
