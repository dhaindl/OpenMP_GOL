#include <iostream>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <string>
#include <chrono>
#include <omp.h>
using namespace std::chrono;
using namespace std;


/*
  Name: Daniel Haindl
  Hw_1: Game of Life OpenMP
  Date: 4/11/2025
  To Compile: g++ -fopenmp GameOfLife.cpp -o GameOfLide
  RunL .\GameOfLife.exe
*/

//Method to update the ghost cells
//Follows the algorithm outlined in the Homework1 pdf
//Copies the bottom and top boundaries, then the sides
void updateGhostCells(vector<vector<bool>>& matrix, int n){
  int i,j;
  for(i =0; i<n; i++){
    matrix[0][i+1] = matrix[n][i+1];
    matrix[n+1][i+1] = matrix[1][i+1];
  }

  for(i =0; i<n+2; i++){
    matrix[i][0] = matrix[i][n];
    matrix[i][n+1] = matrix[i][1];
  }
}

//Method to check if a cell is alive or dead
//If there are two or three neighbors, returns alive
//If there are zero, one or more than 3, returns dead
int isAlive(int row, int column, vector<vector<bool>>& matrix){
  int count = 0;
  int i,j;
  
  //checks the 8 neighbors
  //if at [0,0], continue (don't want to add the own cell)
  for(i = -1; i<=1; i++){
    for(j = -1; j<=1; j++){
      if(i==0 && j==0){
        continue;
      }
      count += matrix[row+i][column+j];
      if(count > 3){
        return 0;
      } 
    }
  }

  if(count == 0 || count == 1){
    return 0;
  }
  else{
    return 1;
  }
}

//Method to update the matrix between generations
//Updates the GhostCells at the end of the method
void updateMatrix(vector<vector<bool>>& matrix, vector<vector<bool>>& matrixCopy,  int n, int numThreads){

  //omp parallel, uses collapse two to multithread both for loops
  #pragma omp parallel for num_threads(numThreads) collapse(2)
  for(int i = 1; i <= n; i++){
    for(int j = 1; j <= n; j++){
      matrixCopy[i][j] = isAlive(i,j,matrix);
    }
  }
  updateGhostCells(matrixCopy, n);

  return;
}


int main() {
  

  int gridSize;
  int numGenerations;
  int numThreads;
  cout << "Size of the grid: ";
  cin >> gridSize;
  cout << "Number of generations: ";
  cin >> numGenerations; 
  cout << "Number of threads: ";
  cin >> numThreads;


  vector<vector<bool>> matrix(gridSize+2, vector<bool>(gridSize+2)); // Original Matrix, uses vectors for dynamic sizing
  srand(time(0)); //seeding rand with time

  //initialize matrix with random values
  for(int i = 0; i < gridSize; i++){
    for( int j = 0; j < gridSize; j++){

      //pads ghost cells initially with 0's
      if(i == 0 || j == 0){
        matrix[i][j] = 0;
      }
      else if(i == gridSize - 1 || j == gridSize - 1){
        matrix[i][j] = 0;
      }

      else{

        //seeded with mostly 0's, evenly distrubted array had massive perefomance issues
        int random = rand() % 1000 + 1;
        if(random > 2){
          matrix[i][j] = 0;
        }
        else{
          matrix[i][j] = 1;
        }
      }
    }
  }

  //update ghost cells and copy to auxillary matrix
  updateGhostCells(matrix, gridSize);
  vector<vector<bool>> matrixCopy = matrix;

  //get the current time
  auto start = high_resolution_clock::now();  

  //for loop to iterate through generations
  //breaks if the board doesn't change state
  for(int i = 0; i < numGenerations; i++){
    updateMatrix(matrix, matrixCopy, gridSize, numThreads);
    if(matrix == matrixCopy){
      break;
    }
    matrix.clear(); 
    matrix = matrixCopy;
  }

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<seconds>(stop - start); 
  

  ofstream myFile;
  myFile.open("out.txt", std::ios::app);

  myFile << "Number threads: " + to_string(numThreads) + " Size of board:" + to_string(gridSize) + "  Number of generations:" + to_string(numGenerations) + " Time took:" + to_string(duration.count()) + "(s)\n"; 

  myFile.close();

  return(0);
}
