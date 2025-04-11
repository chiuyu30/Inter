#include<stdio.h>

#define SIZE 8

void bubbleSort(int *);
void swap(int *, int *);
void printArray(int *);
void SelectionSort(int *);
void InsertionSort(int *);

int main() {
	int data[SIZE] = {2,20,7,19,36,3,23,21};
	printf("Origin:"); 
	printArray(data);
	printf("\n");
	//bubbleSort(data);//µ¥©óbubbleSort(&array[0])
	//SelectionSort(data);
	InsertionSort(data);
	printf("Sort:");
	printArray(data);
	return 0;
}

void bubbleSort(int *array) {
	int flag;
	for (int i = 0; i < SIZE - 1; i++)
	{
		flag = 1;
		for (int j = 0; j < SIZE - 1 - i; j++)
		{
			if (array[j] > array[j + 1]) 
			{
				swap(&array[j], &array[j + 1]);
				flag = 0;
			}
		}
		if (flag) {
			break;
		}
	}
}

void SelectionSort(int *array) {
	int index;
	for (int i = 0; i < SIZE - 1; i++) {
		index = i;
		for (int j = i + 1; j < SIZE; j++) {
			if (array[j] < array[index]) {
				index = j;
			}
		}
		swap(&array[i], &array[index]);
	}
}

void InsertionSort(int *array) {
	int temp;
	for (int i = 0; i < SIZE - 1; i++) {
		int j = 0;
		while (j < i) {

			j++;
		}
	}
}

void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void printArray(int *array) {
	for (int i = 0; i < SIZE; i++) {
		printf("%d ",array[i]);
	}
}