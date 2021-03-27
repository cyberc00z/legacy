// Each According to their Need , Each According to their ability.

/*
 * BANKER'S ALGORITHM => it's a resource allocation and deadlock avoidance algorithm in operating systems.
 * n = processes in system
 * m = number of resources types
 
 *     Banker's Algorithm consists of safety algo and Resource Request Algo.
 *  
 *  SAFETY ALGORITHM
 * How to find out whether system is in a s-state(safe state) or not:?
 *  1. Let work and Finish be vectors of length m and n respectively:
 *      initialize: Work = Avaiable
 *      Finish[i] =false; for i=1,2,3,4...n (i is index)
 *
 *  2. Find an i such that both
 *    a. Finish[i] = False
 *    b. Need(i) <= Work
 *    if no such i exists goto step(4)
 *
 *  3. Work = Work + Allocation[i]
 *     Finish[i] = true
 *     goto step (2) 
 *
 *  4. if Finish[i] = true or all i then the system is in a safe state
 *
 *  RESOURCE REQUEST ALGORITHM.
 *   Request<i> be request array for Process P<i>
 *   Request<i>[j] = K means processP<i> wants k instances of resource type R<j> 
 *   SO BASICALLY WHEN A REQUEST FOR RESOURCES IS MADE BY PROCESS P<i> , following actions will take place:
 *
 *
 *   1.   Request<i> <= Need<i>
 *     Goto Step (2); otherwise raise an error condition;
 *     since the process has exceeded it's maximum claim.
 *
 *  2.  if Request<i> <= Avaiable
 *      Goto step(3); otherwise P<i> must wait, since the resources are not available
 *
 * 3.  Have the system pretend to have allocated the requested to process P<i> by modifying the state as follows:
 * Avaiable = Available - Request<i>
 * Allocation<i> = Allocation<i> + Request<i>
 * Need<i> = Need<i> - Request<i>   
 * */

/*    Algo in Working
 *     
 *       Process | Allocation   |  Max     |    Avaiable   |
 *   ____________|____A  B C____| A  B C __|____A  B_  C___|
 *     P<0>      |   0  1  0    | 7  5 3   |               |  
 *               |              |          |    3 3 2      | 
       P<1>      |   2  0  0    | 3 2  2   |               |
 *               |              |          |               |
 *     P<2>      |   3  0  2    | 9 0  2   |               |
 *               |              |          |               |
 *     P<3>      |   2  1  1    | 2 2 2    |               | 
 *               |              |          |               |
 *_____P<4>______|___0_ 0__2____|_4_3_3____|_______________| 
 *
 *
 *
 *
 *
 * Need[i, j] = Max[i,j] - Allocation[i,j]
 *
 * Need Matrix
 *
 * P<0>   7 4 3 
 * P<1>   1 2 2
 * P<2>   6 0 0
 * P<3>   0 1 1 
 * P<4>   4 3 1
 *
 * find out safe state:
 * Step 1.
 *      m = 3, n=5 
 *      Work = Avaiable
 *
 *      Work = 3 3 2
 *               ----------------------------|---|
 *      Finish = false false false false false 
 *                0    1      2    3      4
 *
 *
 * Step 2.
 *     for i=0;
 *          Need<0> = 7 4 3
 *          Work = 3 3 2
 *           Need<0> > Work
 *
 *     Finish is false So P<0> must wait      
 *
 *
 *    for i=1;
 *         Need<1> = 1 2 2
 *         Work = 3 2 2 
 *         Need<1> < Work
 *
 *     Finish is false 
 *     P<1> must be kept in safe squence
 *
 *     Work = Work + Allocation<1>
 *     Work = 5 3 2
 *     
 *   Finish = False    True    False  False  False  
 *      
 *
 * for i=2;
 *        Need<2> = 6  0  0
 *        Work = 5 3  2
 *     
 *       Need<2> > Work
 *   
 *   Finish is false and So P<2> must wait
 *
 * for i=3;
*       Need<3> = 0 1 1 
 *      Work > Need<3>
 *
 *      Finish is True and P<3> must be kept in safe squence
 *
 *
 *  New Work
 *           = 5 3 2  + Allocation<3> 
 *    
 *   Work =  7 4 3 
 *
 *  Finish = false True false True false
 *  
 *
 *  for i = 4;
 *        Need<4> = 4 3 1
 *        Work > Need<4>
 *
 *   Finish is True and P<4> must be kept in safe squence
 *
 * New Work = 
 *              7 4 3  + Allocation<4>
 *   Work = 7 4 5
 *   
 *   Finish = false true false true true
 *
 *   for i=0;
 *       Need<0> =  7 4 3
 *          
 *       Work > Need<0>
 *
 *   Finish is True and P<0> must be in safe squence    
 *   
 *  New Work = 7 4 5 + Allocation<0>
 *     Work = 7  5  5
 *     
 *   for i=2;
 *     
 *     Need<2> = 6 0  0
 *     Work > Need<0>
 *
 *     Finish is True and P<2> must be in safe squence
 *   
 *   New Work = Work + Allocation<2>
 *           10 5 7
 *
 * 
 *   Finish[i] = true for 0<=i<=n;
 *   Hence the system in safe state
 *   
 *    S-Sq = P1, P3, P4, P0, P2
 *
 *-----------------------------------------------------------------------------
 * First initialize with work = avaiable, then if need > work then Finish is false and Process must be wait
 * otherwise if
 *              work > need
 *                       then Finish is True and
 *                       Process must be in safe squence
 *
 *  New Work after every True in Finish index
 *  New Work = Work + Allocation<n>
 *     
 *-----------------------------------------------------------------------------
 * What will happen if process P1 requests one additional instance of resource
 * type A and two instances of resource type C?
 *
 * Use Resource Request Algorithm:
 *                         if request<i> <= Need<i>:
 *                                     then if request<i> <= Available<i>
 *                                      then
 *                                           Avaiable = Avaiable - Request<i>
 *                                           Allocation<i> = Allocation<i> + Request<i>
 *                                           Need<i> = Need<i> - Request<i> 
 *                                                 
 *
 *           TO DECIDE WHETHER THE REQUEST IS GRANTED WE USE :
 *            Request<i> < Need<i>
 *            Request<i> < Avaiable
 *
 *  
 *
 *    Let's code in C
 *
 * */

#include <stdio.h>

int main(){


	// P0, P1, P2,P3, P4 are the processes 

	int n,m,i,j,k;
	n = 5;   // number of processes 
        m = 3 ;  // type of resources
        int alloc[5][3] = {  { 0,1,0 },        //Allocation Matrix
                              {2,0,0}, 	  
	                     { 3,0,0  },
	                      { 2,1,1  },
			      { 0, 0, 2 } 
	
	                 }; 



	int max[5][3] = {  {   7, 5, 3 },     // MAX Matrix
	                   {  3, 2, 2  },     
	                   {  9, 0, 2  },   
	                   {  2 , 2, 2 },
			   {  4, 3, 3  } 
	                 };

	int avail[3] = {  3,2, 2  };   // Avaiable Resources

	int f[n], ans[n], ind = 0;

	for (k=0; k<n; k++){
	
	
		f[k] = 0;

	}
	int need[n][m];
	for (i=0;i<n;i++){
	
		for (j=0;j<m;j++){
		
			need[i][j] = max[i][j] - alloc[i][j];
		}
	}

	int y =0;
	for (k=0;k<5;k++){
	   for (i=0;i<n;i++){
	   
		   if (f[i] == 0){
		   
			   int flag = 0;
			   for (j=0;j<m;j++){
			   
				   if(need[i][j] > avail[j]){
				     flag = 1;
				     break;
				   }
			   }
			   if (flag==0){
			   
				   ans[ind++] = i;
				   for (y=0;y<m;y++){
				   
					   avail[y] += alloc[i][y];
				   }
			           f[i] = 1;	  
			   }
			   
		   }
	   }
	}

        printf("Safe Squence : \n");
        for (i=0;i<n-1;i++)
		printf("P%d ->", ans[i]);
	printf("P%d", ans[n-1]);


	return 0;



    
}
