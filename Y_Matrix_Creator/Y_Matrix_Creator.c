#include<stdio.h>
#include<stdlib.h>


/* Y_Matrix.c
* 
* 선로 임피던스의 값을 읽어들여, 해당 값을 바탕으로 Y-Matrix를 구성한다.
* 
* 저장 공간을 최대한 줄이기 위해, 모든 배열이 아닌 사용되는 배열의 위치와 값만을 저장하였다.
* 또한, 명령어 실행 중 해당 위치의 어드미턴스 값이 0이 된 경우, 다음 입력에서 해당 저장 공간을 다른 값이 사용할 수 있도록 하였다.
* 
* 실행 시, 입력 파일을 인수로 받아, 이를 해석한다.
* 
* 작성자: YHC03
* 최종 수정일: 2024/10/05
*/


// Calculator Operator
#define CALC_PRINT 0
#define CALC_ADD 1
#define CALC_SUBTRACT 2
#define CALC_CLEAR 3

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

/* memoryAddWrite() 함수
*
* 기능: Y-Matrix 메모리에 새로운 값을 추가한다.
* 입력값: yMatrix 주소, cursor(추가할 위치), node1, node2(선로의 연결부), R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/04
*/
void memoryAddWrite(Y_Matrix* yMatrix, long long int cursor, long long int node1, long long int node2, long double R, long double L, long double C)
{
    // yMatrix에 해당 위치에 해당 선로의 연결부를 기록한다.
    yMatrix[cursor].Node1 = node1;
    yMatrix[cursor].Node2 = node2;

    // 두 선로의 위치가 같은 경우(Line 어드미턴스 + Line과 Ground 사이의 Capacitance)
    if (node1 == node2)
    {
        yMatrix[cursor].realNum = RealValCalc(R, L);
        yMatrix[cursor].imagNum = ImagValCalc(R, L) + C / 2;
    }else{ // 두 선로의 위치가 같은 경우(- Line 어드미턴스)
        yMatrix[cursor].realNum = - RealValCalc(R, L);
        yMatrix[cursor].imagNum = - ImagValCalc(R, L);
    }
    return;
}

/* memorySubtractWrite() 함수
*
* 기능: Y-Matrix 메모리에 새로운 값을 음수로 추가한다.
* 입력값: yMatrix 주소, cursor(추가할 위치), node1, node2(선로의 연결부), R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/04
*/
void memorySubtractWrite(Y_Matrix* yMatrix, long long int cursor, long long int node1, long long int node2, long double R, long double L, long double C)
{
    // yMatrix에 해당 위치에 해당 선로의 연결부를 기록한다.
    yMatrix[cursor].Node1 = node1;
    yMatrix[cursor].Node2 = node2;

    // 두 선로의 위치가 같은 경우(Line 어드미턴스 + Line과 Ground 사이의 Capacitance)
    if (node1 == node2)
    {
        yMatrix[cursor].realNum = - RealValCalc(R, L);
        yMatrix[cursor].imagNum = - ImagValCalc(R, L) - C / 2;
    }else{ // 두 선로의 위치가 같은 경우(- Line 어드미턴스)
        yMatrix[cursor].realNum = RealValCalc(R, L);
        yMatrix[cursor].imagNum = ImagValCalc(R, L);
    }
    return;
}

/* memoryAdd() 함수
*
* 기능: Y-Matrix 메모리의 기존값에 새로운 값을 더하고, 해당 어드미턴스 값이 0인 경우 해당 정보를 제거한다.
* 입력값: yMatrix 주소, cursor(추가할 위치), node1, node2(선로의 연결부), R, L, C값
* 출력값 없음
* 최종 수정일: 2024/10/04
*/
void memoryAdd(Y_Matrix* yMatrix, long long int cursor, long long int node1, long long int node2, long double R, long double L, long double C)
{
    // 두 선로의 위치가 같은 경우(Line 어드미턴스 + Line과 Ground 사이의 Capacitance)
    if (node1 == node2)
    {
        yMatrix[cursor].realNum += RealValCalc(R, L);
        yMatrix[cursor].imagNum += ImagValCalc(R, L) + C / 2;
    }else{ // 두 선로의 위치가 같은 경우(- Line 어드미턴스)
        yMatrix[cursor].realNum += -RealValCalc(R, L);
        yMatrix[cursor].imagNum += -ImagValCalc(R, L);
    }

    // 해당 위치의 전체 어드미턴스가 0인 경우, 해당 Node 정보를 제거한다.
    if (yMatrix[cursor].realNum == 0 && yMatrix[cursor].imagNum == 0)
    {
        yMatrix[cursor].Node1 = 0;
        yMatrix[cursor].Node2 = 0;
    }

    return;
}

/* memorySubtract() 함수
*
* 기능: Y-Matrix 메모리의 기존값에 새로운 값을 빼고, 해당 어드미턴스 값이 0인 경우 해당 정보를 제거한다.
* 입력값: yMatrix 주소, cursor(추가할 위치), node1, node2(선로의 연결부), R, L, C값
* 출력값: 해당 위치의 전체 어드미턴스가 0인 여부
* 최종 수정일: 2024/10/04
*/
void memorySubtract(Y_Matrix* yMatrix, long long int cursor, long long int node1, long long int node2, long double R, long double L, long double C)
{
    // 두 선로의 위치가 같은 경우(Line 어드미턴스 + Line과 Ground 사이의 Capacitance)
    if (node1 == node2)
    {
        yMatrix[cursor].realNum -= RealValCalc(R, L);
        yMatrix[cursor].imagNum -= ImagValCalc(R, L) + C / 2;
    }else{ // 두 선로의 위치가 같은 경우(- Line 어드미턴스)
        yMatrix[cursor].realNum -= -RealValCalc(R, L);
        yMatrix[cursor].imagNum -= -ImagValCalc(R, L);
    }

    // 해당 위치의 전체 어드미턴스가 0인 경우, 해당 Node 정보를 제거한다.
    if (yMatrix[cursor].realNum == 0 && yMatrix[cursor].imagNum == 0)
    {
        yMatrix[cursor].Node1 = 0;
        yMatrix[cursor].Node2 = 0;
    }

    return;
}

/* matrixCalc() 함수
*
* 기능: Y-Matrix값을 계산하여 저장하고, 불러온다.
* 입력값: node1, node2(선로의 연결부), R, L, C값, mode(0: 출력, 1: 추가, 2: 제거, 3: 초기화)
* 출력값: 출력 모드(해당 위치의 어드미턴스의 실수값, 허수값), 이외(0, 0)
* 최종 수정일: 2024/10/04
*/
long double* matrixCalc(long long int node1, long long int node2, long double R, long double L, long double C, int mode)
{
    // 계속 저장할 값 선언
    static Y_Matrix* yMatrix; // yMatrix
    static long long int length = 0; // 배열 저장값 길이
    static long double printValue[2] = { 0, 0 }; // Return값. Call-by Reference를 위함

    // 임시 변수 선언
    long long int i; // 반복문
    long long int tmp, unusedLocation = -1; // 각각 swap용 변수, yMatrix에서 미사용 위치를 찾는 변수

    // node1 < node2가 되도록 설정한다.
    if(node1 > node2)
    {
        tmp = node1;
        node1 = node2;
        node2 = tmp;
    }

    // Mode에 따라 실행
    switch(mode)
    {
        case CALC_PRINT: // 출력 Mode

            // yMatrix 배열 길이만큼 반복
            for(i = 0; i < length; i++)
            {
                // 해당 Node의 값을 찾은 경우
                if(yMatrix[i].Node1 == node1 && yMatrix[i].Node2 == node2)
                {
                    // 해당 Node의 R+jX의 값을 반환
                    printValue[0] = yMatrix[i].realNum;
                    printValue[1] = yMatrix[i].imagNum;
                    return printValue;
                }
            }

            // 해당 Node의 값이 없는 경우, 0을 반환
            printValue[0] = 0;
            printValue[1] = 0;
            return printValue;

            break;

        case CALC_ADD: // 더하기 Mode

            if(length <= 0) // 첫 실행의 경우(malloc 이용)
            {
                length = 3; // yMatrix 크기 설정

                // yMatrix 공간 확보
                yMatrix = (Y_Matrix*)malloc(3 * sizeof(Y_Matrix));
                // 공간 확보 실패 시, 강제 종료
                if (yMatrix == NULL)
                {
                    printf("Overflow!\n");
                    exit(1);
                }

                // Node1의 값 추가
                memoryAddWrite(yMatrix, 0, node1, node1, R, L, C);

                // Node2의 값 추가
                memoryAddWrite(yMatrix, 1, node2, node2, R, L, C);

                // Node1 - Node2의 값 추가
                memoryAddWrite(yMatrix, 2, node1, node2, R, L, C);

            }else{ // 2번째 이상의 실행의 경우
                // Node1의 값 추가
                unusedLocation = -1; // 미사용 메모리의 위치를 찾기 위해, cursor 초기화
                // yMatrix 배열 길이만큼 반복
                for(i = 0; i < length; i++)
                {
                    // 미사용 메모리를 찾은 경우
                    if (yMatrix[i].Node1 == 0 && yMatrix[i].Node2 == 0 && unusedLocation == -1)
                    {
                        // 해당 메모리의 위치 기록
                        unusedLocation = i;
                    }

                    // 해당 Node가 저장된 위치를 찾은 경우
                    if(yMatrix[i].Node1 == node1 && yMatrix[i].Node2 == node1)
                    {
                        // 메모리에 해당 값 추가
                        memoryAdd(yMatrix, i, node1, node1, R, L, C);
                        // For-Loop 탈출
                        i = length + 1;
                    }
                }
                // 해당 Node가 저장되지 않은 경우(Memory 추가)
                if(i == length)
                {
                    // 미사용 메모리가 없는 경우
                    if (unusedLocation == -1)
                    {
                        // yMatrix 공간 확보
                        yMatrix = (Y_Matrix*)realloc(yMatrix, (++length) * sizeof(Y_Matrix));
                        // 공간 확보 실패 시, 강제 종료
                        if (yMatrix == NULL)
                        {
                            printf("Overflow!\n");
                            exit(1);
                        }
                    }else{ // 미사용 메모리가 있는 경우
                        // 커서 위치를 미사용 메모리의 위치로 이동
                        i = unusedLocation;
                    }
                    // 메모리에 해당 값 추가
                    memoryAddWrite(yMatrix, i, node1, node1, R, L, C);
            }

                // Node2의 값 추가
                unusedLocation = -1; // 미사용 메모리의 위치를 찾기 위해, cursor 초기화
                // yMatrix 배열 길이만큼 반복
                for(i = 0; i < length; i++)
                {
                    // 미사용 메모리를 찾은 경우
                    if (yMatrix[i].Node1 == 0 && yMatrix[i].Node2 == 0 && unusedLocation == -1)
                    {
                        // 해당 메모리의 위치 기록
                        unusedLocation = i;
                    }

                    // 해당 Node가 저장된 위치를 찾은 경우
                    if(yMatrix[i].Node1 == node2 && yMatrix[i].Node2 == node2)
                    {
                        // 메모리에 해당 값 추가
                        memoryAdd(yMatrix, i, node2, node2, R, L, C);
                        // For-Loop 탈출
                        i = length + 1;
                    }
                }
                // 해당 Node가 저장되지 않은 경우(Memory 추가)
                if(i == length)
                {
                    // 미사용 메모리가 없는 경우
                    if (unusedLocation == -1)
                    {
                        // yMatrix 공간 확보
                        yMatrix = (Y_Matrix*)realloc(yMatrix, (++length) * sizeof(Y_Matrix));
                        // 공간 확보 실패 시, 강제 종료
                        if (yMatrix == NULL)
                        {
                            printf("Overflow!\n");
                            exit(1);
                        }
                    }else{ // 미사용 메모리가 있는 경우
                        // 커서 위치를 미사용 메모리의 위치로 이동
                        i = unusedLocation;
                    }
                    // 메모리에 해당 값 추가
                    memoryAddWrite(yMatrix, i, node2, node2, R, L, C);
                }

                // Node1 - Node2의 값 추가
                unusedLocation = -1; // 미사용 메모리의 위치를 찾기 위해, cursor 초기화
                // yMatrix 배열 길이만큼 반복
                for(i = 0; i < length; i++)
                {
                    // 미사용 메모리를 찾은 경우
                    if (yMatrix[i].Node1 == 0 && yMatrix[i].Node2 == 0 && unusedLocation == -1)
                    {
                        // 해당 메모리의 위치 기록
                        unusedLocation = i;
                    }

                    // 해당 Node가 저장된 위치를 찾은 경우
                    if(yMatrix[i].Node1 == node1 && yMatrix[i].Node2 == node2)
                    {
                        // 메모리에 해당 값 추가
                        memoryAdd(yMatrix, i, node1, node2, R, L, C);
                        // For-Loop 탈출
                        i = length + 1;
                    }
                }
                // 해당 Node가 저장되지 않은 경우(Memory 추가)
                if(i == length)
                {
                    // 미사용 메모리가 없는 경우
                    if (unusedLocation == -1)
                    {
                        // yMatrix 공간 확보
                        yMatrix = (Y_Matrix*)realloc(yMatrix, (++length) * sizeof(Y_Matrix));
                        // 공간 확보 실패 시, 강제 종료
                        if (yMatrix == NULL)
                        {
                            printf("Overflow!\n");
                            exit(1);
                        }
                    }else{ // 미사용 메모리가 있는 경우
                        // 커서 위치를 미사용 메모리의 위치로 이동
                        i = unusedLocation;
                    }
                    // 메모리에 해당 값 추가
                    memoryAddWrite(yMatrix, i, node1, node2, R, L, C);
                }

            }

            break;

        case CALC_SUBTRACT:  // 빼기 Mode
            if (length <= 0) // 첫 실행의 경우(malloc 이용)
            {
                length = 3; // yMatrix 크기 설정

                // yMatrix 공간 확보
                yMatrix = (Y_Matrix*)malloc(3 * sizeof(Y_Matrix));
                // 공간 확보 실패 시, 강제 종료
                if (yMatrix == NULL)
                {
                    printf("Overflow!\n");
                    exit(1);
                }

                // Node1의 값 추가
                memorySubtractWrite(yMatrix, 0, node1, node1, R, L, C);

                // Node2의 값 추가
                memorySubtractWrite(yMatrix, 1, node2, node2, R, L, C);

                // Node1 - Node2의 값 추가
                memorySubtractWrite(yMatrix, 2, node1, node2, R, L, C);
            }else{ // 2번째 이상의 실행의 경우
                // Node1의 값 추가
                unusedLocation = -1; // 미사용 메모리의 위치를 찾기 위해, cursor 초기화
                // yMatrix 배열 길이만큼 반복
                for (i = 0; i < length; i++)
                {
                    // 미사용 메모리를 찾은 경우
                    if (yMatrix[i].Node1 == 0 && yMatrix[i].Node2 == 0 && unusedLocation == -1)
                    {
                        // 해당 메모리의 위치 기록
                        unusedLocation = i;
                    }

                    // 해당 Node가 저장된 위치를 찾은 경우
                    if (yMatrix[i].Node1 == node1 && yMatrix[i].Node2 == node1)
                    {
                        // 메모리에 해당 값 감산
                        memorySubtract(yMatrix, i, node1, node1, R, L, C);
                        // For-Loop 탈출
                        i = length + 1;
                    }
                }
                // 해당 Node가 저장되지 않은 경우(Memory 추가)
                if (i == length)
                {
                    // 미사용 메모리가 없는 경우
                    if (unusedLocation == -1)
                    {
                        // yMatrix 공간 확보
                        yMatrix = (Y_Matrix*)realloc(yMatrix, (++length) * sizeof(Y_Matrix));
                        // 공간 확보 실패 시, 강제 종료
                        if (yMatrix == NULL)
                        {
                            printf("Overflow!\n");
                            exit(1);
                        }
                    }else{ // 미사용 메모리가 있는 경우
                        // 커서 위치를 미사용 메모리의 위치로 이동
                        i = unusedLocation;
                    }
                    // 메모리에 해당 값 감산
                    memorySubtractWrite(yMatrix, i, node2, node2, R, L, C);
                }

                // Node2의 값 추가
                unusedLocation = -1; // 미사용 메모리의 위치를 찾기 위해, cursor 초기화
                // yMatrix 배열 길이만큼 반복
                for (i = 0; i < length; i++)
                {
                    // 미사용 메모리를 찾은 경우
                    if (yMatrix[i].Node1 == 0 && yMatrix[i].Node2 == 0 && unusedLocation == -1)
                    {
                        // 해당 메모리의 위치 기록
                        unusedLocation = i;
                    }

                    // 해당 Node가 저장된 위치를 찾은 경우
                    if (yMatrix[i].Node1 == node2 && yMatrix[i].Node2 == node2)
                    {
                        // 메모리에 해당 값 감산
                        memorySubtract(yMatrix, i, node2, node2, R, L, C);
                        // For-Loop 탈출
                        i = length + 1;
                    }
                }
                // 해당 Node가 저장되지 않은 경우(Memory 추가)
                if (i == length)
                {
                    // 미사용 메모리가 없는 경우
                    if (unusedLocation == -1)
                    {
                        // yMatrix 공간 확보
                        yMatrix = (Y_Matrix*)realloc(yMatrix, (++length) * sizeof(Y_Matrix));
                        // 공간 확보 실패 시, 강제 종료
                        if (yMatrix == NULL)
                        {
                            printf("Overflow!\n");
                            exit(1);
                        }
                    }else{ // 미사용 메모리가 있는 경우
                        // 커서 위치를 미사용 메모리의 위치로 이동
                        i = unusedLocation;
                    }
                    // 메모리에 해당 값 감산
                    memorySubtractWrite(yMatrix, i, node2, node2, R, L, C);
                }

                // Node1 - Node2의 값 추가
                unusedLocation = -1; // 미사용 메모리의 위치를 찾기 위해, cursor 초기화
                // yMatrix 배열 길이만큼 반복
                for (i = 0; i < length; i++)
                {
                    // 미사용 메모리를 찾은 경우
                    if (yMatrix[i].Node1 == 0 && yMatrix[i].Node2 == 0 && unusedLocation == -1)
                    {
                        // 해당 메모리의 위치 기록
                        unusedLocation = i;
                    }

                    // 해당 Node가 저장된 위치를 찾은 경우
                    if (yMatrix[i].Node1 == node1 && yMatrix[i].Node2 == node2)
                    {
                        // 메모리에 해당 값 감산
                        memorySubtract(yMatrix, i, node1, node2, R, L, C);
                        // For-Loop 탈출
                        i = length + 1;
                    }
                }
                // 해당 Node가 저장되지 않은 경우(Memory 추가)
                if (i == length)
                {
                    // 미사용 메모리가 없는 경우
                    if (unusedLocation == -1)
                    {
                        // yMatrix 공간 확보
                        yMatrix = (Y_Matrix*)realloc(yMatrix, (++length) * sizeof(Y_Matrix));
                        // 공간 확보 실패 시, 강제 종료
                        if (yMatrix == NULL)
                        {
                            printf("Overflow!\n");
                            exit(1);
                        }
                    }else{ // 미사용 메모리가 있는 경우
                        // 커서 위치를 미사용 메모리의 위치로 이동
                        i = unusedLocation;
                    }
                    // 메모리에 해당 값 감산
                    memorySubtractWrite(yMatrix, i, node2, node2, R, L, C);
                }

            }
            break;

        case CALC_CLEAR: // 초기화 Mode
            free(yMatrix); // yMatrix 배열 초기화
            length = 0; // yMatrix 배열 길이 초기화
            break;
    }

    // 입력 및 초기화 모드에서는, [0, 0]을 출력
    printValue[0] = 0;
    printValue[1] = 0;
    return printValue;
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
    long long int node1 = 0, node2 = 0, max_length = 0; // 순서대로 node1, node2값, 배열의 최대 크기(node1, node2의 최대 크기)
    long double* ans; // matrixCalc() 함수의 출력값 저장

    // 파일의 위치 저장
    char filePath[1024] = "";

    // 파일 주소에 빈칸이 있는 경우, 이를 처리한다.
    for (int i = 1; i < argc; i++)
    {
        strcat(filePath, argv[i]);
        strcat(filePath, " ");
    }

    // 파일 열기
    FILE *inputFile = fopen(filePath, "r");
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
        matrixCalc(node1, node2, R, L, C, CALC_ADD);

        // node의 번호가 최댓값인 경우, 해당 값을 max_length 변수에 저장한다.
        if (node1 > max_length)
            max_length = node1;

        if (node2 > max_length)
            max_length = node2;
    }

    // 파일 닫기
    fclose(inputFile);

    // 행렬의 크기를 출력한다.
    printf("%lld*%lld Matrix\n", max_length, max_length);

    // 행렬을 모두 출력한다.
    for (int i = 0; i < max_length; i++)
    {
        for (int j = 0; j < max_length; j++)
        {
            ans = matrixCalc(1+i, 1+j, 0, 0, 0, CALC_PRINT);
            printf("%14.6lf%c%14.6lfj ", ans[0], ans[1]>=0?'+':'-', ans[1] >= 0 ? ans[1] : -ans[1]);
        }
        printf("\n");
    }

    // Y-Matrix 행렬 초기화
    matrixCalc(1, 1, 0, 0, 0, CALC_CLEAR);

    return 0;
}