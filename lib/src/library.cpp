#include "../include/library.h"

#include <cctype>
#include <iostream>
#include <string>

namespace lib
{
    static int partition(std::string& str, int left, int right) {
        // Выбираем опорный элемент
        char pivot = str[left];
        int i = left + 1;

        // Сдвигаем элементы справа, пока не найдём элемент меньше опорного
        for (int j = left + 1; j <= right; ++j) {
            if (str[j] > pivot) {
                std::swap(str[i], str[j]);
                ++i;
            }
        }
        std::swap(str[left], str[i - 1]);
        return i - 1;
    }

    // Рекурсивная быстрая сортировка
    static void quickSort(std::string& str, int left, int right) {
        if (left < right) {
            int pivotIndex = partition(str, left, right);
            quickSort(str, left, pivotIndex - 1);
            quickSort(str, pivotIndex + 1, right);
        }
    }

    //Замена чётного числа на "KB"
    static void substitution(std::string& str) {
        for (std::size_t i = 0; i < str.length(); ++i) {
            int digit = str[i] - '0';
            if (digit % 2 == 0) {
                str.replace(i, 1, "KB");
                ++i;
            }
        }
    }

    //Сортирует элементы входной строки по убыванию и все четные элементы заменяет на латинские буквы «КВ»
    void processString(std::string& str)
    {
        quickSort(str, 0, str.length() - 1);
        substitution(str);
    }
    
    int calculateSum(const std::string& str)
    {
        int sum = 0;
        for (char c : str) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                sum += c - '0';
            }
        }
        return sum;
    }

    void ping() {
        std::cout << "pong1" << std::endl;
    }


    bool checkSum(int sum)
    {
        return std::to_string(sum).length() > 2 && sum % 32 == 0;
    }
}
