# Y_Matrix_Creator
#
# 주어진 File의 Line Impedence 값과 Line-Ground Capacitance값을 바탕으로, Y-Matrix를 구성한다.
#
# inputData = open("Connection.txt", 'r') 구문의 "Connection.txt"의 파일명을 수정하여 이용하세요.
#
# 작성자: YHC03
# 최종 수정일: 2024/10/11


# calRealValue() 함수
#
# 기능: 주어진 저항성분 R+jX에 대한 어드미턴스 실수부를 구한다.
# 입력값: R, jX
# 출력값: 어드미턴스 실수부
# 최종 수정일: 2024/10/11
def calRealValue(R, jX):
    return R/(R*R + jX*jX)

# calImagValue() 함수
#
# 기능: 주어진 저항성분 R+jX에 대한 어드미턴스 허수부를 구한다.
# 입력값: R, jX
# 출력값: 어드미턴스 허수부
def calImagValue(R, jX):
    return -jX/(R*R + jX*jX)

# Y_Matrix 클래스
# 
# 기능: 특정 전력계통의 Y-Matrix값을 저장한다.
# 최종 수정일: 2024/10/11
class Y_Matrix:
    # 생성자
    def __init__(self):
        # Y_Matrix_Val: Y Matrix의 값을 저장하는 변수
        self.Y_Matrix_Val = []

    # 소멸자
    def __del__(self):
        # Y_Matrix_Val List 초기화
        self.Y_Matrix_Val.clear()
        self.Y_Matrix_Val = []

    # findValue() 함수
    # 
    # 기능: Y_Matrix_Val List에서 특정 node의 값을 가진 값의 위치를 찾는다.
    # 입력값: node1, node2
    # 출력값: Y_Matrix_Val List에서 해당 node의 값이 저장된 위치(저장되지 않은 경우, -1 반환)
    # 최종 수정일: 2024/10/11
    def findValue(self, node1, node2):
        for i in range(0, len(self.Y_Matrix_Val)):
            if(self.Y_Matrix_Val[i][0] == node1 and self.Y_Matrix_Val[i][1] == node2):
                return i

        return -1
   
    # clearValue() 함수
    # 기능: Y_Matrix_Val List에서 특정 위치의 값이 0인지 확인하고, 그런 경우 특정 위치의 값을 제거한다.
    # 입력값: location
    # 출력값 없음
    # 최종 수정일: 2024/10/11
    def clearValue(self, location):
        # location 위치에 있는 Y-Matrix값이 0인 경우, 해당 값을 Y_Matrix_Val List에서 제거한다.
        if(self.Y_Matrix_Val[location][2] == 0 and self.Y_Matrix_Val[location][3] == 0):
            self.Y_Matrix_Val.remove(self.Y_Matrix_Val[location])
        
        return
    
    # addValue() 함수
    # 
    # 기능: Y-Matrix에 특정 값을 추가한다. 입력된 두 node가 같은 경우와 다른 경우를 구분한다.
    # 입력값: node1, node2, R, L, C값
    # 출력값 없음
    # 최종 수정일: 2024/10/11
    def addValue(self, node1, node2, R, jX, C):
        # node1과 node2가 같은 경우와 다른 경우를 구분하여, 해당하는 값을 저장한다
        if(node1 == node2):
            realValue = calRealValue(R, jX)
            imagValue = calImagValue(R, jX) + C/2
        else:
            realValue = -calRealValue(R, jX)
            imagValue = -calImagValue(R, jX)

        # 해당 node가 저장된 위치를 찾는다
        location = self.findValue(node1, node2)

        # 해당 node가 저장되어 있지 않은 경우
        if(location == -1):
            # 새로운 값을 추가한다
            self.Y_Matrix_Val.append([node1, node2, realValue, imagValue])
        else: # 해당 node가 저장되어 있는 경우
            # 기존 값에 추가한다
            self.Y_Matrix_Val[location][2] += realValue
            self.Y_Matrix_Val[location][3] += imagValue

            # 만일, 계산 결과가 모두 0인 경우, 해당 Data를 제거한다
            self.clearValue(location)
        
        return

    # subtractValue() 함수
    # 
    # 기능: Y-Matrix에 특정 값을 감산한다. 입력된 두 node가 같은 경우와 다른 경우를 구분한다.
    # 입력값: node1, node2, R, L, C값
    # 출력값 없음
    # 최종 수정일: 2024/10/11
    def subtractValue(self, node1, node2, R, jX, C):
        # node1과 node2가 같은 경우와 다른 경우를 구분하여, 해당하는 값을 저장한다
        if(node1 == node2):
            realValue = calRealValue(R, jX)
            imagValue = calImagValue(R, jX) + C/2
        else:
            realValue = -calRealValue(R, jX)
            imagValue = -calImagValue(R, jX)

        # 해당 node가 저장된 위치를 찾는다
        location = self.findValue(node1, node2)

        # 해당 node가 저장되어 있지 않은 경우
        if(location == -1):
            # 새로운 값을 추가한다
            self.Y_Matrix_Val.append([node1, node2, -realValue, -imagValue])
        else:
            # 기존 값에서 계산한 값을 감산한다
            self.Y_Matrix_Val[location][2] -= realValue
            self.Y_Matrix_Val[location][3] -= imagValue

            # 만일, 계산 결과가 모두 0인 경우, 해당 Data를 제거한다
            self.clearValue(location)
        
        return

    # addValuePackage() 함수
    #
    # 기능: Y-Matrix에 특정 node의 값을 추가하도록 한다.
    # 입력값: node1, node2, R, L, C값
    # 출력값 없음
    # 최종 수정일: 2024/10/11
    def addValuePackage(self, node1, node2, R, jX, C):
        # node1 > node2인 경우, 두 값을 서로 바꾼다
        if(node1 > node2):
            node1, node2 = node2, node1
        
        # node1, node2, node1 - node2에 주어진 값을 추가한다
        self.addValue(node1, node1, R, jX, C)
        self.addValue(node2, node2, R, jX, C)
        self.addValue(node1, node2, R, jX, C)

        return

    # subtractValuePackage() 함수
    #
    # 기능: Y-Matrix에 특정 node의 값을 감산하도록 한다.
    # 입력값: node1, node2, R, L, C값
    # 출력값 없음
    # 최종 수정일: 2024/10/11
    def subtractValuePackage(self, node1, node2, R, jX, C):
        # node1 > node2인 경우, 두 값을 서로 바꾼다
        if(node1 > node2):
            node1, node2 = node2, node1
        
        # node1, node2, node1 - node2에 주어진 값을 감산한다
        self.subtractValue(node1, node1, R, jX, C)
        self.subtractValue(node2, node2, R, jX, C)
        self.subtractValue(node1, node2, R, jX, C)

        return

    # getValue() 함수
    #
    # 기능: Y-Matrix에서 특정 node의 값을 읽어온다.
    # 입력값: node1, node2
    # 출력값: 해당 node의 어드미턴스의 실수값과 허수값
    # 최종 수정일: 2024/10/11
    def getValue(self, node1, node2):
        # node1 > node2인 경우, 두 값을 서로 바꾼다
        if(node1 > node2):
            node1, node2 = node2, node1

        # 해당 node가 저장된 위치를 찾는다
        location = self.findValue(node1, node2)

        # 해당 node가 Y_Matrix_Val에 저장되어 있지 않은 경우
        if(location == -1):
            # 0을 반환한다
            return [0, 0]
        else: # 해당 node가 Y_Matrix_Val에 저장되어 있는 경우
            # 저장된 값을 반환한다
            return [self.Y_Matrix_Val[location][2], self.Y_Matrix_Val[location][3]]
        


# main()

# Y-Matrix 크기의 최댓값을 저장하는 변수
maxValue = 0

# Class 호출
yMatrix = Y_Matrix()

# 파일 열기
try:
    inputData = open("Connection.txt", 'r')
except: # 파일 열기 실패 시
    # 오류 출력 및 프로그램 종료
    print("File Not Found!")
    exit()

# 파일 모두 읽기
dataValue = inputData.readlines()
# 파일 닫기
inputData.close()

# 파일의 Data를 줄마다 분류한다
for i in range(0, len(dataValue)):
    processedValue = dataValue[i].strip().split(' ')

    # 해당 줄의 Data를 내용별로 분류한다
    node1_in = int(processedValue[0])
    node2_in = int(processedValue[1])
    R_in = float(processedValue[2])
    jX_in = float(processedValue[3])
    C_in = float(processedValue[4])

    # 해당 줄의 Data를 Y-Matrix에 반영한다
    yMatrix.addValuePackage(node1_in, node2_in, R_in, jX_in, C_in)

    # Y-Matrix 크기의 최댓값을 확인한다
    if(node1_in > maxValue):
        maxValue = node1_in

    if(node2_in > maxValue):
        maxValue = node2_in


# Y-Matrix의 크기를 출력한다
print(f'{maxValue}*{maxValue} Matrix')

# Y-Matrix의 내용을 모두 출력한다
for i in range(0, maxValue):
    for j in range(0, maxValue):
        [realValue, imagValue] = yMatrix.getValue(i + 1, j + 1)
        print('{:14.6f}'.format(realValue), end = '')
        if(imagValue >= 0):
            print("+j", end = '')
            print('{:14.6f}'.format(imagValue), end = '  ')
        else:
            print("-j", end = '')
            print('{:14.6f}'.format(-imagValue), end = '  ')
    print('')

# End of main()