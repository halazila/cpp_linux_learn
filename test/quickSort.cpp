#include <iostream>
#include <vector>
#include <thread>

using namespace std;

template <class T>
void quickSort(vector<T> &nums, int left, int right)
{
    if (left < right)
    {
        int i = left, j = right;
        T p = nums[left];
        while (i < j)
        {
            while (i < j && p <= nums[j])
                j--;
            while (i < j && nums[i] <= p)
                i++;
            swap(nums[i], nums[j]);
        }
        swap(nums[left], nums[i]);
        quickSort(nums, left, i - 1);
        quickSort(nums, i + 1, right);
    }
}

int main(int argc, char *argv[])
{
    vector<int> nums{3, 0, 1};
    quickSort(nums, 0, nums.size() - 1);
    for (int i = 0; i < nums.size(); i++)
        cout << nums[i] << ",";
    cout << endl;
    return 0;
}