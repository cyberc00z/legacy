/*   Simulaton of Banking Algo
 *
 *   Implementation in C 
 *   Project
 * */

#include <stdio.h>


int main(){


	int n,m,i,j,k;
	 n= 4;
	 m = 2;

	 int alloc[4][2] = {
	                     {  2, 4   },
	                      { 0, 4   },
	                      { 5, 3  },    //Allocation Maxtrix
	                      { 2, 2  }
	 
	                   };

	int max[4][2] = {
		           {3,3},
			   {5,4 },     //MAX Matrix
			   {6,3 },  
			   { 0, 1 }   };


       int avi[2] = { 5, 6  }; //Avaiable Resources
  
       int f[n], ans[n], ind = 0;
       for (k=0;k<n;k++){
       
	       f[k] = 0;
        }

       int need[n][m];
       for (i=0;i<n;i++)
       {
       
	       for (j=0;j<m;j++){
	       
		       need[i][j] = max[i][j] - alloc[i][j];
	       }
       }

       int y= 0;
       for (k=0;k<4;k++){
       
	       for (i=0;i<n;i++){
	       
		       if (f[i]==0){
		       
			       int flag = 0;
			       for (j=0;j<m;j++){
			       
				       if(need[i][j] > avi[j]){
				       
					       flag = 1;
					       break;
				       }
			       }
			       if (flag == 0){
			       
				       ans[ind++] = i;
				       for(y=0;y<m;y++)
					       avi[y] += alloc[i][y];
			               f[i] = 1;
			       } 
		       }
	       }
       }
          
    printf("Safe Squence is : \n") ;
  for (i=0;i<n-1;i++) {
  
	  printf("P%d -> ", ans[i]);
  } 
  printf("P%d", ans[n-1]);

  return 0;
}

 
