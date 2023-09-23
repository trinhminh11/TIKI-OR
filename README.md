1.	Mã hóa lời giải
Một lời giải bao gồm n truck, trong đó:
-	Mỗi truck bao gồm 1 tập các process
-	Trong đó 1 process là công việc thực hiện nhận hoặc gửi 1 package có định danh là id (s_id, r_id)

Ví dụ: 1 lời giải của 3 truck (T1, T2) cần phải vận chuyển 5 gói hàng (p1(0,2), p2(0,3), p3(1,3), p4(1,3), p5(2,3)). Trong đó pi(a,b) là gói hàng thứ i có id hub gửi là a và id hub nhận là b:
-	T1: s_0,s_1,s_2, r_1, s_5, r_5, r_2 ,r_0;
-	T2: s_0,s_3, s_4, r_3, r_4,r_0;
Với s_0, r_0 là một gói hàng chuyển từ depot về depot có khối lượng, thể tích là 0.
2.	Giải mã lời giải
-	Từ list Process của Truck, tạo thành list Nodes các đường đi. 
-	Một Node sẽ chứa thời gian đến, thời gian đi và các Process mà Node đấy cần gửi/nhận.
-	Trên list Node các đường đi đấy nếu có 2 Node liền kề là giống nhau thì merge lại vào 1 Node.
-	In ra độ dài của list Nodes sẽ là độ dài các Node mà truck đi qua.
  -	Với mỗi Node, in ra địa chỉ, số lượng Process, thời gian đến và thời gian đi.
  -	Với mỗi Process của Node, in ra Process ID, thời gian gửi/nhận.
-	Lưu ý: bỏ qua ko in ra lời giải của s_0, r_0 (một Package ảo có địa chỉ gửi từ depot đến depot)
Ví dụ: 1 lời giải của truck (T1) cần phải vận chuyển 5 gói hàng (p1(0,2), p2(0,3), p3(1,3), p5(2,3)) từ hub 1. Trong đó pi(a,b) là gói hàng thứ i có id hub gửi là a và id hub nhận là b:
-	T1: s_0,s_1,s_2, r_1, s_3, r_3, r_2, r_0
-	T1.Nodes: 1, 0, 0, 2, 1, 3, 3, 1 = 1, 0, 2, 1, 3, 1;
-	Độ dài Nodes = 6,
    -	1 0 s_0timeIn s_0timeOut
    -	0 2 s_1timeIn s_2timeOut
    -	1 s_1timIn
    -	2 s_2timIn
    -	2 1 r_1timeIn r_1timeOut
    -	1 r_1timeIn
    -	1 1 s_3timeIn s_3timeOut
    -	3 s_3timeIn
    -	3 2 r3_timeIn r2_timeOut
    - 3 r_3timeIn
    - 2 r_2timeOut
    - 1 0 r_0timeIn r_0timeOut

Với s_0, r_0 là một gói hàng chuyển từ depot về depot có khối lượng, thể tích là 0. Và sẽ bỏ qua không in ra Process s_0 và r_0.

3.	Lời giải khởi tạo
-	Khởi tạo list process có 2 phần tử là [s_0, r_0] là một Package được gửi từ deport đến deport với khối lượng, thể tích đều bằng 0.
4.	Khung thuật toán
Proceduce:
Input:
-	Init tập xe: trucks, tập hub: hubs, tập gọi hàng: packages
Main:
S_best = Initial_first_solution()
While time < time_Move1 :
S_temp = S_best
Move1(S_temp)
S_best = max_serve(S_temp, S_best)
While time out:
S_temp = S_best
Move2(S_temp)
S_best = max_serve(S_temp, S_best)
Output:
-	 Tập process cho mỗi truck

a.	Khởi tạo lời giải:
-	Lặp qua từng Package, rồi lặp qua từng Truck, chạy phép Insert.
b.	Phép Insert:
-	Method Insert của Truck nhận một Package và sẽ add cặp Process gửi và Process nhận của Package đó vào tất cả mọi vị trí có thể trong đường đi hiện tại (thỏa mãn Process gửi luôn đứng trước Process nhận), mỗi lần add đều check lại constraints của bài toán và chọn ra kết quả có tổng thời gian ngắn nhất để add vào.

c.	Phép move 1 <Greedy Insert>
-	Tính cost của từng Package dựa theo thời gian giữa 2 Process của Package và Process kế bên chúng trong list Process của từng truck.
-	Xóa chọn số lượng ngẫu nhiên các Package trong tập 25% Package có cost lớn nhất trong 1 lời giải và xóa chúng khỏi tập lời giải.
-	Shuffle Packages và sort Truck theo số lượng thành phần trong list Process của Truck đó theo thứ tự lớn đến bé
-	Lặp qua từng Package chưa được serve, rồi lặp qua từng Truck, chạy phép Insert.
  
d.	Phép move 2 <n-opt Move>
-	Shuffle Trucks, rồi lặp qua Trucks.
-	Chọn ra n Trucks một lần, xóa đi 25% số Package mà các Trucks đó serve.
-	Trong n Trucks đã chọn, lặp qua các Package mà chưa được gửi, chọn ra Package để add vào Route của Truck sao cho tổng thời gian di chuyển là nhỏ nhất đến khi nào không add được thêm Package nào nữa vào Truck.
  

e.  Các hyper-paramter cần setting
-	Time_move_1: thời gian chạy phép move 1
-	n-opt: số lượng n;
