#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<vector>


/* Y_Matrix.c
*
* 주어진 전력계통에 있는 Line의 Impedence 값과 Line과 Ground간의 Capacitance 값을 읽어들여, 해당 값을 바탕으로 Y-Matrix를 구성한다.
* 
* 저장 공간을 최대한 줄이기 위해, 모든 배열이 아닌 사용되는 배열의 위치와 값만을 저장하였다.
* 또한, 명령어 실행 중 해당 위치의 어드미턴스 값이 0이 된 경우, 해당 값이 저장되어 있던 공간을 제거하도록 하였다.
* 
* 실행 시, 입력 파일을 인수로 받아, 이를 해석한다.
* 
* 작성자: YHC03
* 최종 수정일: 2024/10/05
*/


/* Y_Matrix_Val 구조체
 *
 * 인수: node1, node2(연결된 Node), G, jB(어드미턴스의 실수값과 허수값)
 * 최종 수정일: 2024/10/05
*/
typedef struct
{
    long long int node1, node2;
    long double G, jB;
}Y_Matrix_Val;

/* RealValCalc() 함수
*
* 기능: 주어진 저항성분 R+jX에 대한 어드미턴스 실수부를 구한다.
* 입력값: R, X
* 출력값: 어드미턴스 실수부
* 최종 수정일: 2024/10/05
*/
inline long double RealValCalc(long double R, long double X)
{
    return R / (R * R + X * X);
}

/* ImagValCalc() 함수
*
* 기능: 주어진 저항성분 R+jX에 대한 어드미턴스 허수부를 구한다.
* 입력값: R, X
* 출력값: 어드미턴스 허수부
* 최종 수정일: 2024/10/05
*/
inline long double ImagValCalc(long double R, long double X)
{
    return (-1) * X / (R * R + X * X);
}

/* Y_Matrix 클래스
* 
* 기능: 특정 전력계통의 Y_Matrix값을 저장한다.
* 최종 수정일: 2024/10/05
*/
class Y_Matrix{
private:
    // Y-Matrix의 0이 아닌 값을 저장하는 vector
    std::vector<Y_Matrix_Val> lineValue;

    long long int findMatrix(long long int Node1, long long int Node2);


    bool isValueEmpty(long long int cursor);

public:
Y_Matrix();
~Y_Matrix();

void addMatrix(long long int Node1, long long int Node2, long double R, long double L, long double C);

void subtractMatrix(long long int Node1, long long int Node2, long double R, long double L, long double C);

void addMatrixPackage(long long int Node1, long long int Node2, long double R, long double L, long double C);

void subtractMatrixPackage(long long int Node1, long long int Node2, long double R, long double L, long double C);

long double* getMatrix(long long int Node1, long long int Node2);

};

/* Y_Matrix의 생성자
 *
 * 특별한 기능 없음
 * 최종 수정일: 2024/10/05
*/
Y_Matrix::Y_Matrix()
{
    return;
}

/* Y_Matrix의 소멸자
* 
* 기능: lineValue Vector 초기화
* 최종 수정일: 2024/10/05
*/
Y_Matrix::~Y_Matrix()
{
    // 벡터 초기화
    lineValue.clear();

    return;
}

/* Y_Matrix::findMatrix() 함수
* 
* 기능: lineValue Vector에서 특정 node의 값을 가진 값의 위치를 찾는다.
* 입력값: node1, node2
* 출력값: lineValue Vector에서 해당 node의 값이 저장된 위치(저장되지 않은 경우, -1 반환)
* 최종 수정일: 2024/10/05
*/
long long int Y_Matrix::findMatrix(long long int Node1, long long int Node2)
{
    // vector의 길이만큼 반복
    for (long long int i = 0; i < lineValue.size(); i++)
    {
        // 해당 node의 값을 찾은 경우, 해당 위치 출력
        if (lineValue[i].node1 == Node1 && lineValue[i].node2 == Node2)
        {
            return i;
        }
    }
    // 해당 node의 값이 저장되지 않은 경우, -1 출력
    return -1;
}

/* Y_Matrix::isValueEmpty() 함수
*
* 기능: lineValue Vector에서 특정 위치의 값이 0인지 확인한다.
* 입력값: cursor
* 출력값: 특정 위치의 어드미턴스 값이 0인 경우 참을, 그렇지 않은 경우 거짓을 반환한다.
* 최종 수정일: 2024/10/05
*/
bool Y_Matrix::isValueEmpty(long long int cursor)
{
    // 특정 위치의 어드미턴스 값이 0인 경우 참을, 그렇지 않은 경우 거짓을 반환한다.
    return (lineValue[cursor].G == 0 && lineValue[cursor].jB == 0);
}

/* Y_Matrix::addMatrix() 함수
* 
* 기능: Y-Matrix에 특정 값을 추가한다. 입력된 두 node가 같은 경우와 다른 경우를 구분한다.
* 입력값: Node1, Node2, R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/05
*/
void Y_Matrix::addMatrix(long long int Node1, long long int Node2, long double R, long double L, long double C)
{
    // 값을 저장하기 위한 yMatrixVal 생성
    Y_Matrix_Val yMatrixVal;

    // 어드미턴스 값을 미리 계산하기 위한 변수 생성
    long double realVal, imagVal;

    // 저장할 값의 위치를 찾기
    long long int location = findMatrix(Node1, Node2);

    // 두 node가 같은 경우
    if (Node1 == Node2)
    {
        realVal = RealValCalc(R, L);
        imagVal = ImagValCalc(R, L) + C / 2;
    }else{ // 두 node가 다른 경우
        realVal = -RealValCalc(R, L);
        imagVal = -ImagValCalc(R, L);
    }

    // 해당 node가 저장된 위치가 존재하는 경우
    if (location != -1)
    {
        // 계산한 값을 추가한다.
        lineValue[location].G += realVal;
        lineValue[location].jB += imagVal;

        // 추가된 값이 0이 된 경우, 해당 데이터를 vector에서 지운다.
        if (isValueEmpty(location))
        {
            lineValue.erase(lineValue.begin() + location);
        }
    }else{ // 해당 node가 저장된 위치가 존재하지 않는 경우
        // vector에 해당 값을 추가하기 위해 Y_Matrix_Val 구조체 형식으로 저장한다.
        yMatrixVal.node1 = Node1;
        yMatrixVal.node2 = Node2;
        yMatrixVal.G = realVal;
        yMatrixVal.jB = imagVal;

        // 해당 값을 vector에 추가한다.
        lineValue.push_back(yMatrixVal);
    }

    return;
}

/* Y_Matrix::subtractMatrix() 함수
*
* 기능: Y-Matrix에 특정 값을 감산한다. 입력된 두 node가 같은 경우와 다른 경우를 구분한다.
* 입력값: Node1, Node2, R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/05
*/
void Y_Matrix::subtractMatrix(long long int Node1, long long int Node2, long double R, long double L, long double C)
{
    // 값을 저장하기 위한 yMatrixVal 생성
    Y_Matrix_Val yMatrixVal;

    // 어드미턴스 값을 미리 계산하기 위한 변수 생성
    long double realVal, imagVal;

    // 저장할 값의 위치를 찾기
    long long int location = findMatrix(Node1, Node2);

    // 두 node가 같은 경우
    if (Node1 == Node2)
    {
        realVal = RealValCalc(R, L);
        imagVal = ImagValCalc(R, L) + C / 2;
    }else{ // 두 node가 다른 경우
        realVal = -RealValCalc(R, L);
        imagVal = -ImagValCalc(R, L);
    }

    // 해당 node가 저장된 위치가 존재하는 경우
    if (location != -1)
    {
        // 계산한 값을 감산한다.
        lineValue[location].G -= realVal;
        lineValue[location].jB -= imagVal;

        // 추가된 값이 0이 된 경우, 해당 데이터를 vector에서 지운다.
        if (isValueEmpty(location))
        {
            lineValue.erase(lineValue.begin() + location);
        }
    }else{ // 해당 node가 저장된 위치가 존재하지 않는 경우
        // vector에 해당 값을 0에서 감산하기 위해 Y_Matrix_Val 구조체 형식으로 저장한다.
        yMatrixVal.node1 = Node1;
        yMatrixVal.node2 = Node2;
        yMatrixVal.G = -realVal;
        yMatrixVal.jB = -imagVal;

        // 해당 값을 vector에 추가한다.
        lineValue.push_back(yMatrixVal);
    }

    return;
}

/* Y_Matrix::addMatrixPackage() 함수
*
* 기능: Y-Matrix에 특정 node의 값을 추가하도록 한다.
* 입력값: Node1, Node2, R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/05
*/
void Y_Matrix::addMatrixPackage(long long int Node1, long long int Node2, long double R, long double L, long double C)
{
    // Swap 실행용 변수 선언
    long long int tmp;

    // node1 > node2인 경우, 두 값을 서로 바꾼다.
    if (Node1 > Node2)
    {
        tmp = Node1;
        Node1 = Node2;
        Node2 = tmp;
    }

    // node1 추가
    addMatrix(Node1, Node1, R, L, C);
    // node2 추가
    addMatrix(Node2, Node2, R, L, C);
    // node1 - node2 추가
    addMatrix(Node1, Node2, R, L, C);

    return;
}

/* Y_Matrix::subtractMatrixPackage() 함수
*
* 기능: Y-Matrix에 특정 node의 값을 감산하도록 한다.
* 입력값: Node1, Node2, R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/05
*/
void Y_Matrix::subtractMatrixPackage(long long int Node1, long long int Node2, long double R, long double L, long double C)
{
    // Swap 실행용 변수 선언
    long long int tmp;

    // node1 > node2인 경우, 두 값을 서로 바꾼다.
    if (Node1 > Node2)
    {
        tmp = Node1;
        Node1 = Node2;
        Node2 = tmp;
    }

    // node1 감산
    subtractMatrix(Node1, Node1, R, L, C);
    // node2 감산
    subtractMatrix(Node2, Node2, R, L, C);
    // node1 - node2 감산
    subtractMatrix(Node1, Node2, R, L, C);

    return;
}

/* Y_Matrix::getMatrix() 함수
*
* 기능: Y-Matrix에서 특정 node의 값을 읽어온다.
* 입력값: Node1, Node2
* 출력값: 해당 node의 어드미턴스의 실수값과 허수값
* 최종 수정일: 2024/10/05
*/
long double* Y_Matrix::getMatrix(long long int Node1, long long int Node2)
{
    // 출력값을 저장하기 위한 변수 선언(Call-by Reference를 위해 static으로 설정)
    static long double matrixVal[2] = {0, 0};

    // 내부 변수 선언
    long long int tmp, location; // 순서대로 Swap용 변수, lineValue vector에서 원하는 값의 위치를 나타내는 변수

    // node1 > node2인 경우, 두 값을 서로 바꾼다.
    if (Node1 > Node2)
    {
        tmp = Node1;
        Node1 = Node2;
        Node2 = tmp;
    }

    // 입력받은 두 node의 값을 가지고 있는 위치를 찾는다.
    location = findMatrix(Node1, Node2);

    // 두 node의 값이 저장되어 있는 경우
    if (location != -1)
    {  
        // 해당 값을 반환한다.
        matrixVal[0] = lineValue[location].G;
        matrixVal[1] = lineValue[location].jB;
        return matrixVal;
    }

    // 두 node의 값이 저장되어 있지 않은 경우, 0을 반환한다.
    matrixVal[0] = 0;
    matrixVal[1] = 0;
    return matrixVal;
}

/*
* main() 함수
* 기능: File을 읽고, Y-Matrix을 계산해 출력한다.
* 입력값: file 인수
* 최종 수정일: 2024/10/05
*/
void main(int argc, char* argv[])
{
    // Class 호출
    Y_Matrix yMatrix1;

    // 변수 선언
    long long int node1, node2, maxLength = 0; // 순서대로 읽어들인 node1, node2값, node의 최댓값
    long double R, L, C, *ans; // 순서대로 읽어들인 R, L, C값, 해당 위치의 어드미턴스값을 저장하는 위치를 가리키는 포인터
    
    // 파일의 위치 저장
    std::string filePath = "";
    // 파일 주소에 빈칸이 있는 경우, 이를 처리한다.
    for (int i = 1; i < argc; i++)
    {
        filePath += argv[i];
        filePath += " ";
    }
    // 파일 열기
    std::ifstream inputFile(filePath);
    // 파일 열기 실패 시, 프로그램을 종료한다.
    if (!inputFile.is_open())
    {
        std::cout << "File Not Found" << std::endl;
        return;
    }

    // 파일 끝까지 반복
    while (!inputFile.eof())
    {
        // 파일의 해당 줄 읽기
        inputFile >> node1 >> node2 >> R >> L >> C;
        // 읽은 데이터를 바탕으로, Y-Matrix 계산 및 저장을 진행한다.
        yMatrix1.addMatrixPackage(node1, node2, R, L, C);

        // node의 번호가 최댓값인 경우, 해당 값을 maxLength 변수에 저장한다.
        if (node1 > maxLength)
            maxLength = node1;

        if (node2 > maxLength)
            maxLength = node2;
    }

    // 파일 닫기
    inputFile.close();

    // 행렬의 크기를 출력한다.
    std::cout << maxLength << "*" << maxLength << " Matrix" << std::endl;

    // 행렬을 모두 출력한다.
    for (int i = 0; i < maxLength; i++)
    {
        for (int j = 0; j < maxLength; j++)
        {
            ans = yMatrix1.getMatrix(i + 1, j + 1);
            std::cout << std::setw(14) << std::fixed << std::setprecision(6) << ans[0] << (ans[1] >= 0 ? '+' : '-') << (ans[1] >= 0 ? ans[1] : -ans[1]) << "  ";
        }
        std::cout << std::endl;
    }

    // Class 내용 초기화
    yMatrix1.~Y_Matrix();

    return;
}