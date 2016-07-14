; ModuleID = 'support/test.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [12 x i8] c"Hello World\00", align 1
@hw = global i8* getelementptr inbounds ([12 x i8]* @.str, i32 0, i32 0), align 8
@y = global i32 100, align 4
@.str1 = private unnamed_addr constant [17 x i8] c"Enter a number: \00", align 1
@.str2 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str3 = private unnamed_addr constant [17 x i8] c"You entered: %d\0A\00", align 1
@.str4 = private unnamed_addr constant [10 x i8] c"%s %d %u\0A\00", align 1
@.str5 = private unnamed_addr constant [10 x i8] c"%d %s %u\0A\00", align 1
@.str6 = private unnamed_addr constant [8 x i8] c"j: %d \0A\00", align 1
@bbCounter = internal global i32 0
@BasicBlockPrintfFormatStr = private constant [14 x i8] c"BB Count: %d\0A\00"
@bbCounter0 = internal global i32 0
@BasicBlockPrintfFormatStr1 = private constant [8 x i8] c"b0: %d\0A\00"
@bbCounter1 = internal global i32 0
@BasicBlockPrintfFormatStr2 = private constant [8 x i8] c"b1: %d\0A\00"
@bbCounter2 = internal global i32 0
@BasicBlockPrintfFormatStr3 = private constant [8 x i8] c"b2: %d\0A\00"
@bbCounter3 = internal global i32 0
@BasicBlockPrintfFormatStr4 = private constant [8 x i8] c"b3: %d\0A\00"
@bbCounter4 = internal global i32 0
@BasicBlockPrintfFormatStr5 = private constant [8 x i8] c"b4: %d\0A\00"
@bbCounter5 = internal global i32 0
@BasicBlockPrintfFormatStr6 = private constant [8 x i8] c"b5: %d\0A\00"
@bbCounter6 = internal global i32 0
@BasicBlockPrintfFormatStr7 = private constant [8 x i8] c"b6: %d\0A\00"
@bbCounter7 = internal global i32 0
@BasicBlockPrintfFormatStr8 = private constant [8 x i8] c"b7: %d\0A\00"

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
b0:
  %0 = load i32* @bbCounter
  %1 = add i32 1, %0
  store i32 %1, i32* @bbCounter
  %2 = load i32* @bbCounter0
  %3 = add i32 1, %2
  store i32 %3, i32* @bbCounter0
  %4 = alloca i32, align 4
  %x = alloca i32, align 4
  %j = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %4
  store i32 0, i32* %x, align 4
  %5 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([17 x i8]* @.str1, i32 0, i32 0))
  %6 = call i32 (i8*, ...)* @__isoc99_scanf(i8* getelementptr inbounds ([3 x i8]* @.str2, i32 0, i32 0), i32* %x)
  %7 = load i32* %x, align 4
  %8 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([17 x i8]* @.str3, i32 0, i32 0), i32 %7)
  %9 = load i32* %x, align 4
  %10 = load i32* @y, align 4
  %11 = icmp ugt i32 %9, %10
  br i1 %11, label %b1, label %b2

b1:                                               ; preds = %b0
  %12 = load i32* @bbCounter
  %13 = add i32 1, %12
  store i32 %13, i32* @bbCounter
  %14 = load i32* @bbCounter1
  %15 = add i32 1, %14
  store i32 %15, i32* @bbCounter1
  %16 = load i8** @hw, align 8
  %17 = load i32* %x, align 4
  %18 = load i32* @y, align 4
  %19 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([10 x i8]* @.str4, i32 0, i32 0), i8* %16, i32 %17, i32 %18)
  br label %b3

b2:                                               ; preds = %b0
  %20 = load i32* @bbCounter
  %21 = add i32 1, %20
  store i32 %21, i32* @bbCounter
  %22 = load i32* @bbCounter2
  %23 = add i32 1, %22
  store i32 %23, i32* @bbCounter2
  %24 = load i32* %x, align 4
  %25 = load i8** @hw, align 8
  %26 = load i32* @y, align 4
  %27 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([10 x i8]* @.str5, i32 0, i32 0), i32 %24, i8* %25, i32 %26)
  br label %b3

b3:                                               ; preds = %b2, %b1
  %28 = load i32* @bbCounter
  %29 = add i32 1, %28
  store i32 %29, i32* @bbCounter
  %30 = load i32* @bbCounter3
  %31 = add i32 1, %30
  store i32 %31, i32* @bbCounter3
  store i32 0, i32* %j, align 4
  store i32 0, i32* %i, align 4
  br label %b4

b4:                                               ; preds = %b6, %b3
  %32 = load i32* @bbCounter
  %33 = add i32 1, %32
  store i32 %33, i32* @bbCounter
  %34 = load i32* @bbCounter4
  %35 = add i32 1, %34
  store i32 %35, i32* @bbCounter4
  %36 = load i32* %i, align 4
  %37 = load i32* %x, align 4
  %38 = icmp slt i32 %36, %37
  br i1 %38, label %b5, label %b7

b5:                                               ; preds = %b4
  %39 = load i32* @bbCounter
  %40 = add i32 1, %39
  store i32 %40, i32* @bbCounter
  %41 = load i32* @bbCounter5
  %42 = add i32 1, %41
  store i32 %42, i32* @bbCounter5
  %43 = load i32* %x, align 4
  %44 = load i32* %j, align 4
  %45 = add nsw i32 %44, %43
  store i32 %45, i32* %j, align 4
  br label %b6

b6:                                               ; preds = %b5
  %46 = load i32* @bbCounter
  %47 = add i32 1, %46
  store i32 %47, i32* @bbCounter
  %48 = load i32* @bbCounter6
  %49 = add i32 1, %48
  store i32 %49, i32* @bbCounter6
  %50 = load i32* %i, align 4
  %51 = add nsw i32 %50, 1
  store i32 %51, i32* %i, align 4
  br label %b4

b7:                                               ; preds = %b4
  %52 = load i32* @bbCounter
  %53 = add i32 1, %52
  store i32 %53, i32* @bbCounter
  %54 = load i32* @bbCounter7
  %55 = add i32 1, %54
  store i32 %55, i32* @bbCounter7
  %56 = load i32* %j, align 4
  %57 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @.str6, i32 0, i32 0), i32 %56)
  %58 = load i32* @bbCounter
  %59 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([14 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i32 %58)
  %60 = load i32* @bbCounter0
  %61 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr1, i32 0, i32 0), i32 %60)
  %62 = load i32* @bbCounter1
  %63 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr2, i32 0, i32 0), i32 %62)
  %64 = load i32* @bbCounter2
  %65 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr3, i32 0, i32 0), i32 %64)
  %66 = load i32* @bbCounter3
  %67 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr4, i32 0, i32 0), i32 %66)
  %68 = load i32* @bbCounter4
  %69 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr5, i32 0, i32 0), i32 %68)
  %70 = load i32* @bbCounter5
  %71 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr6, i32 0, i32 0), i32 %70)
  %72 = load i32* @bbCounter6
  %73 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr7, i32 0, i32 0), i32 %72)
  %74 = load i32* @bbCounter7
  %75 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([8 x i8]* @BasicBlockPrintfFormatStr8, i32 0, i32 0), i32 %74)
  ret i32 0
}

declare i32 @printf(i8*, ...) #1

declare i32 @__isoc99_scanf(i8*, ...) #1

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.6.2-1 (tags/RELEASE_362/final) (based on LLVM 3.6.2)"}
