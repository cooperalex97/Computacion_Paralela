echo "Linked-List"
rm LinkedList/ll_run
gcc -g LinkedList/linked_list.c -o LinkedList/ll_run -lpthread
echo "----------------------------"
echo "Matrix-Vector Multiplications"
rm MatrixVectorMul/mv_run
gcc -g MatrixVectorMul/matrix_x_vector.c -o MatrixVectorMul/mv_run -lpthread
echo "----------------------------"
echo "PI"
rm PI/pi_run
gcc -g PI/pi.c -o PI/pi_run -lpthread
echo "----------------------------"
echo "Strtok"
rm Strtock/str_run
gcc -g Strtock/strtok_p.c -o Strtock/str_run -lpthread

