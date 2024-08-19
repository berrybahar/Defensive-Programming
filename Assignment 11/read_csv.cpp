#include <iostream>
#include <fstream>
#include <string>
#include <vector>

bool isCsv(const std::string fileName)
{
    if(fileName.find(".csv") == std::string::npos)
        return false;
    int i;
    char csv [] = {'.', 'c', 's', 'v'};
    for(i = fileName.size() - 1; fileName.at(i) == ' '; i--)
    for(int j = (sizeof(csv) / sizeof(char)) - 1; j != 0; j--)
        if(fileName.at(i) == csv[j])
            i--;
        else return false;

    return true;
}

double printSumOfLine(std::string line, std::vector<std::string>& col, int& colSize)
{
    colSize = 0;
    double sum = 0;
    int digitCounter;
    std::vector<std::string> nums;
    int i = 0;
    while(!isdigit(line.at(i)))
    {
        if(line.at(i) == ',')
            break;
        i++;   
    }
    for(i; i < line.size(); i++)
    {
        digitCounter = 0;
        nums.push_back("");
        while(i < line.size() && line.at(i) && line.at(i) != ',')
        {
            if(isdigit(line.at(i)) || (digitCounter == 0 && line.at(i) == '-')
                ||(i + 1 < line.size() && digitCounter != 0 && line.at(i) == '.' && isdigit(line.at(i + 1))))
            {
                nums[nums.size() - 1] += line.at(i);
                digitCounter++;
            }
            else
            {
                nums[nums.size() - 1] = "";
                while(i < line.size() && line.at(i) != ',')
                    i++;
                break;
            }
            i++;
        }
    }

    for(int i = 0; i < nums.size(); i++)
    {
        colSize++;
        col.push_back(nums[i]);
        if(nums[i] != "")
            sum += std::stod(nums[i]);
    }

    return sum;
}

double printCol(std::vector<std::string>& col, const int colSize, const int colNum)
{
    double colSum = 0;
    for(int i = colNum; i < col.size(); i += colSize)
    {
        if(col[i] != "")
            colSum += std::stod(col[i]);
    }
    return colSum;
}

int main()
{
    std::string fileName;
    while(!isCsv(fileName))
    {
        std::cout<<"Please enter a file name from csv type.\nYou can enter 0 for exiting"<<std::endl;
        std::cin>>fileName;
        if(fileName == "0")
            return 0;
    }
    std::ifstream file(fileName);
    std::string line;
    std::vector<std::string> col;
    int colSize;
    while(std::getline(file, line))
        std::cout<<"Total of line: "<<printSumOfLine(line, col, colSize)<<std::endl;
    std::cout<<"Sums of columns:"<<std::endl;
    for(int i = 0; i < colSize; i++)
        std::cout<<" "<<printCol(col, colSize, i);
    std::cout<<std::endl;
    return 0;
}