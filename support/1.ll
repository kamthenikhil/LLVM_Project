; ModuleID = 'support/1.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@counter = internal global i32 0
@Basic_BlockPrintfFormatStr = private constant [21 x i8] c"\0Afunction_1:\0Ab0: %d\0A\00"
@counter1 = internal global i32 0
@Basic_BlockPrintfFormatStr2 = private constant [8 x i8] c"b1: %d\0A\00"
@counter3 = internal global i32 0
@Basic_BlockPrintfFormatStr4 = private constant [8 x i8] c"b2: %d\0A\00"
@counter5 = internal global i32 0
@Basic_BlockPrintfFormatStr6 = private constant [8 x i8] c"b3: %d\0A\00"
@counter7 = internal global i32 0
@Basic_BlockPrintfFormatStr8 = private constant [8 x i8] c"b4: %d\0A\00"
@counter9 = internal global i32 0
@Basic_BlockPrintfFormatStr10 = private constant [8 x i8] c"b5: %d\0A\00"
@counter11 = internal global i32 0
@Basic_BlockPrintfFormatStr12 = private constant [8 x i8] c"b6: %d\0A\00"
@counter13 = internal global i32 0
@Basic_BlockPrintfFormatStr14 = private constant [8 x i8] c"b7: %d\0A\00"
@counter15 = internal global i32 0
@Basic_BlockPrintfFormatStr16 = private constant [15 x i8] c"\0Amain:\0Ab0: %d\0A\00"
@edgeCounter = internal global [9 x i64] zeroinitializer

; Function Attrs: nounwind uwtable
define void @function_1(i32 %x) #0 {
b0:
  %0 = load i32* @counter
  %1 = add i32 1, %0
  store i32 %1, i32* @counter
  %2 = alloca i32, align 4
  store i32 %x, i32* %2, align 4
  br label %b1

b1:                                               ; preds = %b6, %b0
  %3 = load i32* @counter1
  %4 = add i32 1, %3
  store i32 %4, i32* @counter1
  %5 = load i32* %2, align 4
  %6 = icmp ugt i32 %5, 0
  br i1 %6, label %b3, label %b2

b2:                                               ; preds = %b1
  %7 = load i32* @counter3
  %8 = add i32 1, %7
  store i32 %8, i32* @counter3
  br label %b7

b3:                                               ; preds = %b1
  %9 = load i32* @counter5
  %10 = add i32 1, %9
  store i32 %10, i32* @counter5
  %11 = load i32* %2, align 4
  %12 = urem i32 %11, 4
  %13 = icmp eq i32 %12, 0
  br i1 %13, label %b4, label %b5

b4:                                               ; preds = %b3
  %14 = load i32* @counter7
  %15 = add i32 1, %14
  store i32 %15, i32* @counter7
  %16 = load i32* %2, align 4
  %17 = add i32 %16, -1
  store i32 %17, i32* %2, align 4
  br label %b6

b5:                                               ; preds = %b3
  %18 = load i32* @counter9
  %19 = add i32 1, %18
  store i32 %19, i32* @counter9
  %20 = load i32* %2, align 4
  %21 = add i32 %20, -1
  store i32 %21, i32* %2, align 4
  br label %b6

b6:                                               ; preds = %b5, %b4
  %22 = load i32* @counter11
  %23 = add i32 1, %22
  store i32 %23, i32* @counter11
  br label %b1

b7:                                               ; preds = %b2
  %24 = load i32* @counter13
  %25 = add i32 1, %24
  store i32 %25, i32* @counter13
  ret void
}

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
b0:
  %0 = load i32* @counter15
  %1 = add i32 1, %0
  store i32 %1, i32* @counter15
  %2 = alloca i32, align 4
  store i32 0, i32* %2
  call void @function_1(i32 100)
  %3 = load i32* @counter
  %4 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([21 x i8]* @Basic_BlockPrintfFormatStr, i32 0, i32 0), i32 %3)
  %5 = load i32* @counter1
  %6 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @Basic_BlockPrintfFormatStr2, i32 0, i32 0), i32 %5)
  %7 = load i32* @counter3
  %8 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @Basic_BlockPrintfFormatStr4, i32 0, i32 0), i32 %7)
  %9 = load i32* @counter5
  %10 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @Basic_BlockPrintfFormatStr6, i32 0, i32 0), i32 %9)
  %11 = load i32* @counter7
  %12 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @Basic_BlockPrintfFormatStr8, i32 0, i32 0), i32 %11)
  %13 = load i32* @counter9
  %14 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @Basic_BlockPrintfFormatStr10, i32 0, i32 0), i32 %13)
  %15 = load i32* @counter11
  %16 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @Basic_BlockPrintfFormatStr12, i32 0, i32 0), i32 %15)
  %17 = load i32* @counter13
  %18 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @Basic_BlockPrintfFormatStr14, i32 0, i32 0), i32 %17)
  %19 = load i32* @counter15
  %20 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([15 x i8]* @Basic_BlockPrintfFormatStr16, i32 0, i32 0), i32 %19)
  ret i32 0
}

declare i32 @printf(i8*, ...)

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.6.2-1 (tags/RELEASE_362/final) (based on LLVM 3.6.2)"}
