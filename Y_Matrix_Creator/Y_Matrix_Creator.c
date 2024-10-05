#include<stdio.h>
#include<stdlib.h>


/* Y_Matrix.c
*
* 주어진 전력계통에 있는 Line의 Impedence 값과 Line과 Ground간의 Capacitance 값을 읽어들여, 해당 값을 바탕으로 Y-Matrix를 구성한다.
* 
* 저장 공간을 최대한 줄이기 위해, 모든 배열이 아닌 사용되는 배열의 위치와 값만을 저장하였다.
* 또한, 명령어 실행 중 해당 위치의 어드미턴스 값이 0이 된 경우, 다음 입력에서 해당 저장 공간을 다른 값이 사용할 수 있도록 하였다.
* 
* 실행 시, 입력 파일을 인수로 받아, 이를 해석한다.
* 
* 작성자: YHC03
* 최종 수정일: 2024/10/05
*/


// Y-Matrix 저장소 설정
typedef struct{
    unsigned long long int Node1, Node2;
    long double realNum, imagNum;
}Y_Matrix;

/* RealValCalc() 함수
*
* 기능: 주어진 저항성분 R+jX에 대한 어드미턴스 실수부를 구한다.
* 입력값: R, X
* 출력값: 어드미턴스 실수부
* 최종 수정일: 2024/10/04
*/
long double RealValCalc(long double R, long double X)
{
    return R / (R * R + X * X);
}

/* ImagValCalc() 함수
*
* 기능: 주어진 저항성분 R+jX에 대한 어드미턴스 허수부를 구한다.
* 입력값: R, X
* 출력값: 어드미턴스 허수부
* 최종 수정일: 2024/10/04
*/
long double ImagValCalc(long double R, long double X)
{
    return (-1) * X / (R * R + X * X);
}

/* matrixAdd() 함수
*
* 기능: Y-Matrix 메모리의 기존값에 새로운 값을 더하고, 해당 어드미턴스 값이 0인 경우 해당 정보를 제거한다.
* 입력값: yMatrix 주소, cursor(추가할 위치), node1, node2(선로의 연결부), R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/05
*/
void matrixAdd(Y_Matrix** yMatrix, long long int* length, long long int node1, long long int node2, long double R, long double L, long double C)
{
    long double realVal, imagVal; // 어드미턴스의 실수값과 허수값을 미리 계산해 저장하기 위한 변수
    long long int cursor = 0, firstEnptyLocation = -1; // 각각 읽고 수정할 Y-Matrix의 해당 값의 위치, 첫번째로 위치한 미사용 Y-Matrix값
    
    // 두 선로의 위치가 같은 경우(Line 어드미턴스 + Line과 Ground 사이의 Capacitance)
    if (node1 == node2)
    {
        realVal = RealValCalc(R, L);
        imagVal = ImagValCalc(R, L) + C / 2;
    }else{ // 두 선로의 위치가 다른 경우(- Line 어드미턴스)
        realVal = -RealValCalc(R, L);
        imagVal = -ImagValCalc(R, L);
    }

    // yMatrix 배열 길이만큼 반복
    for (cursor = 0; cursor < *length; cursor++)
    {
        // 미사용 메모리를 찾은 경우
        if ((*yMatrix)[cursor].Node1 == 0 && (*yMatrix)[cursor].Node2 == 0 && firstEnptyLocation == -1)
        {
            // 해당 메모리의 위치 기록
            firstEnptyLocation = cursor;
        }

        // 해당 Node가 저장된 위치를 찾은 경우
        if ((*yMatrix)[cursor].Node1 == node1 && (*yMatrix)[cursor].Node2 == node2)
        {
            // 메모리에 해당 값 추가
            (*yMatrix)[cursor].realNum += realVal;
            (*yMatrix)[cursor].imagNum += imagVal;

            // For-Loop 탈출
            cursor = *length + 1;
        }
    }

    // 해당 Node가 저장되지 않은 경우(Memory 추가)
    if (cursor == *length)
    {
        // 미사용 메모리가 있는 경우
        if (firstEnptyLocation != -1)
        {
            // 비어있는 메모리에 해당 값 추가
            (*yMatrix)[firstEnptyLocation].Node1 = node1;
            (*yMatrix)[firstEnptyLocation].Node2 = node2;
            (*yMatrix)[firstEnptyLocation].realNum = realVal;
            (*yMatrix)[firstEnptyLocation].imagNum = imagVal;
        }else{
            // yMatrix 공간 확보
            *yMatrix = (Y_Matrix*)realloc(*yMatrix, (++*length) * sizeof(Y_Matrix));
            // 공간 확보 실패 시, 강제 종료
            if (*yMatrix == NULL)
            {
                printf("Overflow!\n");
                exit(1);
            }

            // 추가된 메모리에 해당 값 추가
            (*yMatrix)[cursor].Node1 = node1;
            (*yMatrix)[cursor].Node2 = node2;
            (*yMatrix)[cursor].realNum = realVal;
            (*yMatrix)[cursor].imagNum = imagVal;
        }
    }

    // 해당 위치의 전체 어드미턴스가 0인 경우, 해당 Node 정보를 제거한다.
    if ((*yMatrix)[cursor].realNum == 0 && (*yMatrix)[cursor].imagNum == 0)
    {
        (*yMatrix)[cursor].Node1 = 0;
        (*yMatrix)[cursor].Node2 = 0;
    }

    return;
}

/* matrixSubtract() 함수
*
* 기능: Y-Matrix 메모리의 기존값에 새로운 값을 빼고, 해당 어드미턴스 값이 0인 경우 해당 정보를 제거한다.
* 입력값: yMatrix 주소, cursor(추가할 위치), node1, node2(선로의 연결부), R, L, C값
* 출력값: 해당 위치의 전체 어드미턴스가 0인 여부
* 최종 수정일: 2024/10/05
*/
void matrixSubtract(Y_Matrix** yMatrix, long long int* length, long long int node1, long long int node2, long double R, long double L, long double C)
{
    long double realVal, imagVal; // 어드미턴스의 실수값과 허수값을 미리 계산해 저장하기 위한 변수
    long long int cursor = 0, firstEnptyLocation = -1; // 각각 읽고 수정할 Y-Matrix의 해당 값의 위치, 첫번째로 위치한 미사용 Y-Matrix값

    // 두 선로의 위치가 같은 경우(Line 어드미턴스 + Line과 Ground 사이의 Capacitance)
    if (node1 == node2)
    {
        realVal = RealValCalc(R, L);
        imagVal = ImagValCalc(R, L) + C / 2;
    }else{ // 두 선로의 위치가 다른 경우(- Line 어드미턴스)
        realVal = -RealValCalc(R, L);
        imagVal = -ImagValCalc(R, L);
    }

    // yMatrix 배열 길이만큼 반복
    for (cursor = 0; cursor < *length; cursor++)
    {
        // 미사용 메모리를 찾은 경우
        if ((*yMatrix)[cursor].Node1 == 0 && (*yMatrix)[cursor].Node2 == 0 && firstEnptyLocation == -1)
        {
            // 해당 메모리의 위치 기록
            firstEnptyLocation = cursor;
        }
        
        // 해당 Node가 저장된 위치를 찾은 경우
        if ((*yMatrix)[cursor].Node1 == node1 && (*yMatrix)[cursor].Node2 == node2)
        {
            // 메모리에 해당 값 추가
            (*yMatrix)[cursor].realNum -= realVal;
            (*yMatrix)[cursor].imagNum -= imagVal;

            // For-Loop 탈출
            cursor = *length + 1;
        }
    }

    // 해당 Node가 저장되지 않은 경우(Memory 추가)
    if (cursor == *length)
    {
        // 미사용 메모리가 있는 경우
        if (firstEnptyLocation != -1)
        {
            // 비어있는 메모리에 해당 값 추가
            (*yMatrix)[firstEnptyLocation].Node1 = -node1;
            (*yMatrix)[firstEnptyLocation].Node2 = -node2;
            (*yMatrix)[firstEnptyLocation].realNum = -realVal;
            (*yMatrix)[firstEnptyLocation].imagNum = -imagVal;
        }else{
            // yMatrix 공간 확보
            *yMatrix = (Y_Matrix*)realloc(*yMatrix, (++*length) * sizeof(Y_Matrix));
            // 공간 확보 실패 시, 강제 종료
            if (*yMatrix == NULL)
            {
                printf("Overflow!\n");
                exit(1);
            }

            // 추가된 메모리에 해당 값 추가
            (*yMatrix)[cursor].Node1 = -node1;
            (*yMatrix)[cursor].Node2 = -node2;
            (*yMatrix)[cursor].realNum = -realVal;
            (*yMatrix)[cursor].imagNum = -imagVal;
        }
    }

    // 해당 위치의 전체 어드미턴스가 0인 경우, 해당 Node 정보를 제거한다.
    if ((*yMatrix)[cursor].realNum == 0 && (*yMatrix)[cursor].imagNum == 0)
    {
        (*yMatrix)[cursor].Node1 = 0;
        (*yMatrix)[cursor].Node2 = 0;
    }

    return;
}

/* matrixAddPackage() 함수
* 
* 기능: 해당 Y-Matrix에 주어진 Line 임피던스와 선로와 지면 사이의 Capacitance의 값을 추가한다.
* 입력값: yMatrix 주소, length(yMatrix의 길이), node1, node2, R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/05
*/
void matrixAddPackage(Y_Matrix** yMatrix, long long int* length, long long int node1, long long int node2, long double R, long double L, long double C)
{
    // Swap을 위한 변수 선언
    long long int tmp;

    // node1 < node2가 되도록 설정한다.
    if (node1 > node2)
    {
        tmp = node1;
        node1 = node2;
        node2 = tmp;
    }

    // Y-Matrix에 node 1 추가
    matrixAdd(yMatrix, length, node1, node1, R, L, C);
    // Y-Matrix에 node 2 추가
    matrixAdd(yMatrix, length, node2, node2, R, L, C);
    // Y-Matrix에 node 1 - node 2 추가
    matrixAdd(yMatrix, length, node1, node2, R, L, C);

    return;
}

/* matrixSubtractPackage() 함수
*
* 기능: 해당 Y-Matrix에 주어진 Line 임피던스와 선로와 지면 사이의 Capacitance의 값을 제거한다.
* 입력값: yMatrix 주소, length(yMatrix의 길이), node1, node2, R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/05
*/
void matrixSubtractPackage(Y_Matrix** yMatrix, long long int* length, long long int node1, long long int node2, long double R, long double L, long double C)
{
    // Swap을 위한 변수 선언
    long long int tmp;

    // node1 < node2가 되도록 설정한다.
    if (node1 > node2)
    {
        tmp = node1;
        node1 = node2;
        node2 = tmp;
    }

    // Y-Matrix에 node 1 감산
    matrixSubtract(yMatrix, length, node1, node1, R, L, C);
    // Y-Matrix에 node 2 감산
    matrixSubtract(yMatrix, length, node2, node2, R, L, C);
    // Y-Matrix에 node 1 - node 2 감산
    matrixSubtract(yMatrix, length, node1, node2, R, L, C);

    return;
}

/* matrixRead() 함수
* 
* 기능: 주어진 Y-Matrix의 주어진 위치의 값을 반환한다.
* 입력값: yMatrix 주소, length(yMatrix의 길이), node1, node2
* 출력값: Y-Matrix의 해당 위치의 어드미턴스의 실수값과 허수값
* 최종 수정일: 2024/10/05
*/
long double* matrixRead(Y_Matrix** yMatrix, long long int* length, long long int node1, long long int node2)
{
    static long double printValue[2] = { 0, 0 }; // Return값. Call-by Reference를 위함

    // yMatrix 배열 길이만큼 반복
    for (long long int i = 0; i < *length; i++)
    {
        // 해당 Node의 값을 찾은 경우
        if ((*yMatrix)[i].Node1 == node1 && (*yMatrix)[i].Node2 == node2)
        {
            // 해당 Node의 G+jB의 값을 반환
            printValue[0] = (*yMatrix)[i].realNum;
            printValue[1] = (*yMatrix)[i].imagNum;
            return printValue;
        }
    }

    // 해당 Node의 값이 없는 경우, 0을 반환
    printValue[0] = 0;
    printValue[1] = 0;
    return printValue;
}

/* resetMatrix() 함수
* 
* 기능: 주어진 Y-Matrix를 초기화한다
* 입력값: yMatrix 주소, length(yMatrix의 길이)
* 출력값 없음
* 최종 수정일: 2024/10/05
*/
void resetMatrix(Y_Matrix** yMatrix, long long int* length)
{
    // yMatrix 메모리 할당 해제 및 초기화
    free(*yMatrix);
    *yMatrix = NULL;

    // yMatrix의 길이 초기화
    *length = 0;

    return;
}

/*
* main() 함수
* 기능: File을 읽고, Y-Matrix을 계산해 출력한다.
* 입력값: file 인수
* 최종 수정일: 2024/10/05
*/
int main(int argc, char* argv[])
{
    // File 입력값 및 함수 출력값 저장용 변수 선언
    long double R = 0, L = 0, C = 0; // 순서대로 R, L, C값
    long long int node1 = 0, node2 = 0, matrix1Length = 0, maxLength = 0; // 순서대로 node1, node2값, 배열의 길이, 배열의 최대 크기(node1, node2의 최대 크기)
    long double* ans; // matrixRead() 함수의 출력값 저장

    Y_Matrix* yMatrix1 = NULL; // yMatrix 초기화

    // 파일의 위치 저장
    char filePath[1024] = "";

    // 파일 주소에 빈칸이 있는 경우, 이를 처리한다.
    for (int i = 1; i < argc; i++)
    {
        strcat(filePath, argv[i]);
        strcat(filePath, " ");
    }

    // 파일 열기
    FILE* inputFile = fopen(filePath, "r");
    // 파일 열기 실패 시, 프로그램을 종료한다.
    if (inputFile == NULL)
    {
        printf("File Not Found\n");
        return 0;
    }

    // 파일 끝까지 읽기
    while (fscanf(inputFile, "%lld %lld %lf %lf %lf", &node1, &node2, &R, &L, &C) != EOF)
    {
        // 읽은 데이터를 바탕으로, Y-Matrix 계산 및 저장을 진행한다.
        matrixAddPackage(&yMatrix1, &matrix1Length, node1, node2, R, L, C);

        // node의 번호가 최댓값인 경우, 해당 값을 maxLength 변수에 저장한다.
        if (node1 > maxLength)
            maxLength = node1;

        if (node2 > maxLength)
            maxLength = node2;
    }

    // 파일 닫기
    fclose(inputFile);

    // 행렬의 크기를 출력한다.
    printf("%lld*%lld Matrix\n", maxLength, maxLength);

    // 행렬을 모두 출력한다.
    for (int i = 0; i < maxLength; i++)
    {
        for (int j = 0; j < maxLength; j++)
        {
            ans = matrixRead(&yMatrix1, &matrix1Length, i + 1, j + 1);
            printf("%14.6lf%c%14.6lfj ", ans[0], ans[1] >= 0 ? '+' : '-', ans[1] >= 0 ? ans[1] : -ans[1]);
        }
        printf("\n");
    }

    // Y-Matrix 행렬 초기화
    resetMatrix(&yMatrix1, &matrix1Length);

    return 0;
}