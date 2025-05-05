g++ -g -fopenmp A1.cpp -o A1.exe          
./A1.exe                   
!nvcc -arch=sm_75 vec_add.cu -o vec                  
!./vec
