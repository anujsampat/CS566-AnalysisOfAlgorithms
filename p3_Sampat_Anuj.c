/*
 *    Anuj Sampat
 *    asampat@bu.edu
 *
 *    CS 566 Programming Assignment 3: Strongly Connected Components
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTICES	1000000
#define WHITE		-1
#define GRAY		 0
#define BLACK		 1

#define INPUT_FILE_PATH "p3_input.txt"
#define OUTPUT_FILE_PATH "p3_out.txt"
FILE *outputFile;

typedef struct adjVertex_t ADJ_VERTEX;

typedef struct vertex_t
{
  int vertexNum;
  int color, discoverTime, finishTime;
  struct vertex_t *parent;
  struct vertex_t *discoveredComponent;
  ADJ_VERTEX *adjacencyList;
} VERTEX;

struct adjVertex_t
{
  VERTEX *graphVertex;
  struct adjVertex_t *next;
};

typedef struct graph_t
{
  int dfsTime;
  int numVertices;
  VERTEX *vertexList[MAX_VERTICES + 1];	/* As input by user */
  VERTEX *sortedVertexList[MAX_VERTICES + 1];	/* Sorted in increasing order of finish time after depth first search */
  VERTEX **dfsVertexList;	/* Points to either one of the above vertex lists */
  int currSortIndex;
} GRAPH;

static void printSortedVertices (GRAPH *);
static void printAdjacencyList (GRAPH *);

static GRAPH * initializeGraph (void)
{
  GRAPH *newGraph = (GRAPH *) malloc (sizeof (GRAPH));
  int i;

  if (newGraph != NULL)
    {
      newGraph->dfsTime = 0;
      newGraph->numVertices = 0;
      newGraph->currSortIndex = 0;
      newGraph->dfsVertexList = newGraph->vertexList;

      for (i = 0; i <= MAX_VERTICES; i++)
	{
	  newGraph->vertexList[i] = NULL;
	  newGraph->sortedVertexList[i] = NULL;
	}
    }

  return newGraph;
}

static GRAPH * freeGraph (GRAPH * currGraph)
{
  VERTEX *graphVertex;
  ADJ_VERTEX *currAdjVertex, *nextAdjVertex;
  int i;

  if (currGraph == NULL)
    return;

  for (i = 0; i <= MAX_VERTICES; i++)
    {
      graphVertex = currGraph->vertexList[i];
      if (graphVertex != NULL)
	{
	  nextAdjVertex = graphVertex->adjacencyList;
	  while (nextAdjVertex != NULL)
	    {
	      currAdjVertex = nextAdjVertex;
	      nextAdjVertex = currAdjVertex->next;
	      free (currAdjVertex);
	    }

	  free (graphVertex);
	}
    }

  free (currGraph);

  return NULL;
}

static VERTEX * createGraphVertex (int vertexNum)
{
  VERTEX *retVertex = NULL;

  retVertex = (VERTEX *) malloc (sizeof (VERTEX));
  if (retVertex != NULL)
    {
      retVertex->vertexNum = vertexNum;
      retVertex->color = WHITE;
      retVertex->parent = NULL;
      retVertex->discoveredComponent = NULL;
      retVertex->adjacencyList = NULL;
    }

  return retVertex;
}

/*
 * Create the vertices 'srcVertexNum' and 'destVertexNum' in the graph if they
 * don't already exist and connect them via the graph adjacency list.
 *
 * Returns 0 on success, -1 otherwise.
 */
static int createGraphEdge (GRAPH * currGraph, int srcVertexNum, int destVertexNum)
{
  VERTEX *srcVertex, *destVertex;
  ADJ_VERTEX *prevAdjVertex, *currAdjVertex, *newAdjVertex;

  srcVertex = currGraph->vertexList[srcVertexNum];

  if (srcVertex == NULL)
    {
      srcVertex = createGraphVertex (srcVertexNum);
      if (srcVertex == NULL)
	{
	  printf ("Could not allocate node\n");
	  return -1;
	}

      currGraph->vertexList[srcVertexNum] = srcVertex;
      currGraph->numVertices++;
    }

  destVertex = currGraph->vertexList[destVertexNum];
  if (destVertex == NULL)
    {
      destVertex = createGraphVertex (destVertexNum);
      if (destVertex == NULL)
	{
	  printf ("Could not allocate node\n");
	  return -1;
	}

      currGraph->vertexList[destVertexNum] = destVertex;
      currGraph->numVertices++;
    }

  newAdjVertex = (ADJ_VERTEX *) malloc (sizeof (ADJ_VERTEX));
  if (newAdjVertex == NULL)
    {
      printf ("Could not allocate adjacent vertex\n");
      return -1;
    }

  newAdjVertex->graphVertex = destVertex;
  newAdjVertex->next = NULL;

  currAdjVertex = srcVertex->adjacencyList;
  prevAdjVertex = srcVertex->adjacencyList;

  if (currAdjVertex == NULL)
    {
      srcVertex->adjacencyList = newAdjVertex;
    }
  else				/* Insert in increasing order of vertexNum */
    {
      while ((currAdjVertex->graphVertex->vertexNum <=
	      newAdjVertex->graphVertex->vertexNum)
	     && (currAdjVertex->next != NULL))
	{
	  prevAdjVertex = currAdjVertex;
	  currAdjVertex = currAdjVertex->next;
	}

      if (currAdjVertex == NULL)
	prevAdjVertex->next = newAdjVertex;
      else
	{
	  if (prevAdjVertex == currAdjVertex)
	    {
	      if (prevAdjVertex->graphVertex->vertexNum >
		  newAdjVertex->graphVertex->vertexNum)
		{
		  srcVertex->adjacencyList = newAdjVertex;
		  newAdjVertex->next = prevAdjVertex;
		}
	      else
		prevAdjVertex->next = newAdjVertex;
	    }
	  else
	    {
	      prevAdjVertex->next = newAdjVertex;
	      newAdjVertex->next = currAdjVertex;
	    }
	}
    }

  return 0;
}

static void depthFirstVisit (GRAPH * currGraph, VERTEX * currVertex)
{
  ADJ_VERTEX *adjVertex;
  VERTEX *parent;
  int i;

  currGraph->dfsTime += 1;
  currVertex->discoverTime = currGraph->dfsTime;
  currVertex->color = GRAY;

  adjVertex = currVertex->adjacencyList;

  while (adjVertex != NULL)
    {
      if (adjVertex->graphVertex->color == WHITE)
	{
	  adjVertex->graphVertex->parent = currVertex;
	  currVertex->discoveredComponent = adjVertex->graphVertex;
	  depthFirstVisit (currGraph, adjVertex->graphVertex);
	}

      adjVertex = adjVertex->next;
    }

  currVertex->color = BLACK;
  currGraph->dfsTime += 1;
  currVertex->finishTime = currGraph->dfsTime;

  currGraph->sortedVertexList[currGraph->currSortIndex] = currVertex;
  currGraph->currSortIndex++;
}

static void depthFirstSearch (GRAPH * currGraph)
{
  VERTEX *currVertex;
  int i;

  for (i = 0; i <= MAX_VERTICES; i++)
    {
      currVertex = currGraph->dfsVertexList[i];

      if (currVertex != NULL)
	{
	  currVertex->color = WHITE;
	  currVertex->parent = NULL;
	}
    }

  currGraph->dfsTime = 0;

  for (i = 0; i <= MAX_VERTICES; i++)
    {
      currVertex = currGraph->dfsVertexList[i];

      if (currVertex != NULL && currVertex->color == WHITE)
	{
	  depthFirstVisit (currGraph, currVertex);
	}
    }
}

/* This function will do a depth first search of the graph pointed
 * to by 'transposeGraph' in a reverse order of the vertices pointed
 * to by transposeGraph->dfsVertexList.
 *
 */
static void depthFirstSearchTranspose (GRAPH * transposeGraph)
{
  VERTEX *currVertex, *sccVertex;
  ADJ_VERTEX *adjVertex;
  int i, sortedVertexNum;

  for (i = 0; i <= MAX_VERTICES; i++)
    {
      currVertex = transposeGraph->vertexList[i];

      if (currVertex != NULL)
	{
	  currVertex->color = WHITE;
	  currVertex->parent = NULL;
	}
    }

  transposeGraph->dfsTime = 0;

  for (i = transposeGraph->numVertices - 1; i >= 0; i--)
    {
      sortedVertexNum = transposeGraph->dfsVertexList[i]->vertexNum;
      currVertex = transposeGraph->vertexList[sortedVertexNum];

      if (currVertex != NULL && currVertex->color == WHITE)
	{
	  depthFirstVisit (transposeGraph, currVertex);

	  /* Print all the strongly connected components discovered
	   * during the depth first visit.
	   */
	  printf ("Strongly Connected Components Group: ");
	  fprintf (outputFile, "Strongly Connected Components Group: ");
	  sccVertex = currVertex->discoveredComponent;

	  /* Only single node root */
	  if (sccVertex == NULL)
	    {
	      printf ("Node %d (finish time %d)", currVertex->vertexNum,
		      currVertex->finishTime);
	      fprintf (outputFile, "Node %d (finish time %d)",
		       currVertex->vertexNum, currVertex->finishTime);
	    }
	  else			/* Print components in order of finish time */
	    {
	      while (sccVertex != NULL
		     && sccVertex->discoveredComponent != NULL)
		sccVertex = sccVertex->discoveredComponent;
	      while (sccVertex != NULL)
		{
		  printf ("Node %d (finish time %d) ", sccVertex->vertexNum,
			  sccVertex->finishTime);
		  fprintf (outputFile, "Node %d (finish time %d) ",
			   sccVertex->vertexNum, sccVertex->finishTime);
		  sccVertex = sccVertex->parent;
		}
	    }
	  printf ("\n\n");
	  fprintf (outputFile, "\n\n");
	}
    }
}

static GRAPH * getTranspose (GRAPH * currGraph)
{
  GRAPH *transposeGraph = NULL;
  VERTEX *graphVertex;
  ADJ_VERTEX *adjVertex;
  int i;

  transposeGraph = initializeGraph ();
  if (transposeGraph == NULL)
    {
      printf ("Could not allocate transpose graph\n");
    }
  else
    {
      for (i = 0; i <= MAX_VERTICES; i++)
	{
	  graphVertex = currGraph->vertexList[i];

	  if (graphVertex != NULL)
	    {
	      adjVertex = graphVertex->adjacencyList;
	      while (adjVertex != NULL)
		{
		  if (createGraphEdge
		      (transposeGraph, adjVertex->graphVertex->vertexNum,
		       graphVertex->vertexNum) != 0)
		    {
		      printf ("Could not allocate node in transpose graph\n");
		      transposeGraph = freeGraph (transposeGraph);
		      break;
		    }

		  adjVertex = adjVertex->next;
		}
	    }
	}
    }

  return transposeGraph;
}

static void stronglyConnectedComponents (GRAPH * currGraph)
{
  GRAPH *transposeGraph;
  int i, retVal;

  depthFirstSearch (currGraph);

  transposeGraph = getTranspose (currGraph);

  /* DFS on the transpose is performed on the vertices discovered
   * in the previous DFS in the decreasing order of their finish 
   * times. Set dfsVertexList to this sorted list.
   */
  transposeGraph->dfsVertexList = currGraph->sortedVertexList;

  depthFirstSearchTranspose (transposeGraph);
}

static void printAdjacencyList (GRAPH * currGraph)
{
  VERTEX *currVertex;
  ADJ_VERTEX *adjVertex;
  int i;

  printf ("Total Vertices = %d\n", currGraph->numVertices);

  for (i = 0; i <= MAX_VERTICES; i++)
    {
      currVertex = currGraph->vertexList[i];

      if (currVertex != NULL)
	{
	  printf ("\nVertex = %d Discover Time = %d Finish Time = %d\n",
		  currVertex->vertexNum, currVertex->discoverTime,
		  currVertex->finishTime);

	  printf ("Adjacent Vertices = ");
	  adjVertex = currVertex->adjacencyList;
	  while (adjVertex != NULL)
	    {
	      printf ("%d ", adjVertex->graphVertex->vertexNum);
	      adjVertex = adjVertex->next;
	    }

	  printf ("\n");
	}
    }
}

static void printSortedVertices (GRAPH * currGraph)
{
  int i;

  for (i = 0; i < currGraph->currSortIndex; i++)
    {
      printf ("Vertex %d finish time %d\n",
	      currGraph->sortedVertexList[i]->vertexNum,
	      currGraph->sortedVertexList[i]->finishTime);
    }
}

int main (int argc, char *argv[])
{
  FILE *inputFile;
  char inputVertices[MAX_VERTICES];
  char *nodeNum, *adjVertices;
  GRAPH *currGraph;
  int srcVertexNum, destVertexNum;

  if (argc > 1 && argv[1] != NULL)
    inputFile = fopen (argv[1], "r");
  else
    inputFile = fopen (INPUT_FILE_PATH, "r");

  if (inputFile == NULL)
    {
      printf ("Could not open input file\n");
      return -1;
    }

  if (argc > 2 && argv[2] != NULL)
    outputFile = fopen (argv[2], "w");
  else
    outputFile = fopen (OUTPUT_FILE_PATH, "w");

  if (outputFile == NULL)
    {
      printf ("Could not open output file\n");
      fclose (inputFile);
      return -1;
    }

  currGraph = initializeGraph ();
  if (currGraph == NULL)
    {
      printf ("Could not allocate graph\n");
      fclose (inputFile);
      fclose (outputFile);
      return -1;
    }

  while (fgets (inputVertices, MAX_VERTICES, inputFile) != NULL)
    {
      if (currGraph->numVertices == MAX_VERTICES)
	{
	  printf ("Maximum vertices exceeded\n");
	  freeGraph (currGraph);
	  fclose (inputFile);
	  fclose (outputFile);
	  return -1;
	}

      nodeNum = strtok (inputVertices, ":");
      if (nodeNum != NULL)
	{
	  sscanf (nodeNum, "%d", &srcVertexNum);

	  while ((adjVertices = strtok (NULL, ",")) != NULL)
	    {
	      sscanf (adjVertices, "%d", &destVertexNum);
	      if (createGraphEdge (currGraph, srcVertexNum, destVertexNum) !=
		  0)
		{
		  printf ("Could not create adjacent node\n");
		  freeGraph (currGraph);
		  fclose (inputFile);
		  fclose (outputFile);
		  return -1;
		}
	    }
	}
    }

  printf ("\n\n");

  stronglyConnectedComponents (currGraph);

  freeGraph (currGraph);
  fclose (inputFile);
  fclose (outputFile);

  return 0;
}
