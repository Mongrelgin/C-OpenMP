#include <iostream>
#include <omp.h>

using namespace std;

void rnd()
{
	for (int i = 0; i < 100000; i++)
		rand();
}

int main()
{
	//omp_set_num_threads(2);

	/*2. Экспериментально подтвердите параллельную работу двух
	потоков при использовании директив for, sections. Подтвердите,
	используя директиву OMP_GET_NUM_THREADS. Объясните и
	покажите использование директивы barrier*/

	cout << "1 threads" << endl;
	for (int i = 0; i < 10; i++)
		cout << omp_get_num_threads();

	cout << "\n2 threads\n";
#pragma omp parallel for //Создание параллельных циклов
	for (int i = 0; i < 10; i++)
		cout << omp_get_num_threads();

	cout << "\nUse section\n";
#pragma omp parallel//Создание параллельных секций
	{
#pragma omp sections nowait//nowait - отключает неявный барьер в конце sections
		{
#pragma omp section //Создание фрагмента программы, не зависящего от другого фрагмента, но имеющего зависимости внутри себя
			{
				for (int i = 0; i < 10; i++) {
					cout << 1;
				}
			}
#pragma omp section
			{
				for (int i = 0; i < 30; i++) {
					cout << 2;
				}
			}
		}
#pragma omp barrier //Включает барьер, теперь pragma omp sections запустится только когда потоки в sections выше будут завершены

#pragma omp sections
		{
#pragma omp section
			{
				for (int i = 0; i < 5; i++) {
					cout << 3;
				}
			}
#pragma omp section
			{
				for (int i = 0; i < 15; i++) {
					cout << 4;
				}
			}
		}
	}

	/*3. Напишите многопотоковую программу с применением двух
	директив (single, critical, master, atomic, ordered… )
	синхронизации потоков (директивы синхронизации у студентов
	должны отличаться). Программа может выводить на экран номер
	потока или заполнять массив из двух потоков с использованием
	директив синхронизации*/

	cout << "\nCritical\n";
#pragma omp parallel for
	for (int i = 0; i < 10; i++) {
#pragma omp critical//Код в critical выполняется только по одному потоку за раз, в противном случае возможно непредсказуемое поведение
		cout << 1 << " ";
	}

#pragma omp parallel for
	for (int i = 0; i < 10; i++)
		cout << 2 << " ";//Непредсказуемое поведение (Могут пропасть " ", т.к. к потоку cout могут обратится сразу два потока)

	cout << "\nMaster\n";

	int sum[10];

#pragma omp parallel
	{
#pragma omp for
		for (int i = 0; i < 5; i++)
			sum[i] = i * i;

#pragma omp for
		for (int i = 5; i < 10; i++)
			sum[i] = i * i * i;


#pragma omp master//раздел кода должен выполняться в одном потоке
		{
			for (int i = 0; i < 10; i++)
				cout << sum[i] << " ";
			cout << endl;
		}

	}

	/*4. Напишите многопотоковую программу с применением
	синхронизации на базе замков OMP_INIT_LOCK(var),
	OMP_SET_LOCK, OMP_UNSET_LOCK, OMP_TEST_LOCK.*/

	int counter = 0;

	cout << "Init Lock: ";

	omp_lock_t lock;
	int i, p_sum = 0, res = 0;

	omp_init_lock(&lock);
#pragma omp parallel firstprivate(p_sum)
	{
#pragma parallel for private(i)
		for (i = 0; i < 10; i++)
			p_sum += i;
		omp_set_lock(&lock);//Заставляет вызвавшую нить дождаться освобождения замка, а затем захватывает его
		res += p_sum;
		omp_unset_lock(&lock);//Освобождает замок, если он был захвачен вызвавшей нитью
	}
	omp_destroy_lock(&lock);//Уничтожает замок, связанный с переменной var
	cout << res;
}