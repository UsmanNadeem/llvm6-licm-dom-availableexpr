; ModuleID = 'tests/licm3.ll'
source_filename = "tests/licm3.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [15 x i8] c"result is: %d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define i32 @main() #0 {
  %1 = alloca [100 x [100 x double]], align 16
  %2 = alloca [100 x double], align 16
  %3 = bitcast [100 x [100 x double]]* %1 to i8*
  call void @llvm.memset.p0i8.i64(i8* %3, i8 0, i64 80000, i32 16, i1 false)
  %4 = bitcast i8* %3 to [100 x [100 x double]]*
  %5 = getelementptr [100 x [100 x double]], [100 x [100 x double]]* %4, i32 0, i32 0
  %6 = getelementptr [100 x double], [100 x double]* %5, i32 0, i32 0
  store double 1.300000e+01, double* %6
  %7 = bitcast [100 x double]* %2 to i8*
  call void @llvm.memset.p0i8.i64(i8* %7, i8 0, i64 800, i32 16, i1 false)
  %8 = bitcast i8* %7 to [100 x double]*
  %9 = getelementptr [100 x double], [100 x double]* %8, i32 0, i32 0
  store double 1.000000e+00, double* %9
  br label %10

; <label>:10:                                     ; preds = %65, %0
  %.02 = phi i32 [ 0, %0 ], [ %66, %65 ]
  %.01 = phi i32 [ 0, %0 ], [ %.1, %65 ]
  %11 = icmp slt i32 %.02, 100
  br i1 %11, label %12, label %67

; <label>:12:                                     ; preds = %10
  br label %13

; <label>:13:                                     ; preds = %62, %12
  %.03 = phi i32 [ 0, %12 ], [ %63, %62 ]
  %.1 = phi i32 [ %.01, %12 ], [ %.2, %62 ]
  %14 = icmp slt i32 %.03, 10
  br i1 %14, label %15, label %64

; <label>:15:                                     ; preds = %13
  br label %16

; <label>:16:                                     ; preds = %59, %15
  %.2 = phi i32 [ %.1, %15 ], [ %58, %59 ]
  %.0 = phi i32 [ 0, %15 ], [ %60, %59 ]
  %17 = icmp slt i32 %.0, 100
  br i1 %17, label %18, label %61

; <label>:18:                                     ; preds = %16
  %19 = sext i32 %.02 to i64
  %20 = getelementptr inbounds [100 x [100 x double]], [100 x [100 x double]]* %1, i64 0, i64 %19
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds [100 x double], [100 x double]* %20, i64 0, i64 %21
  %23 = load double, double* %22, align 8
  %24 = sext i32 %.0 to i64
  %25 = getelementptr inbounds [100 x [100 x double]], [100 x [100 x double]]* %1, i64 0, i64 %24
  %26 = sext i32 %.02 to i64
  %27 = getelementptr inbounds [100 x double], [100 x double]* %25, i64 0, i64 %26
  %28 = load double, double* %27, align 8
  %29 = fmul double %23, %28
  %30 = sitofp i32 %.2 to double
  %31 = fadd double %30, %29
  %32 = fptosi double %31 to i32
  %33 = sext i32 %.03 to i64
  %34 = getelementptr inbounds [100 x [100 x double]], [100 x [100 x double]]* %1, i64 0, i64 %33
  %35 = sext i32 %.0 to i64
  %36 = getelementptr inbounds [100 x double], [100 x double]* %34, i64 0, i64 %35
  %37 = load double, double* %36, align 8
  %38 = sext i32 %.0 to i64
  %39 = getelementptr inbounds [100 x [100 x double]], [100 x [100 x double]]* %1, i64 0, i64 %38
  %40 = sext i32 %.03 to i64
  %41 = getelementptr inbounds [100 x double], [100 x double]* %39, i64 0, i64 %40
  %42 = load double, double* %41, align 8
  %43 = fmul double %37, %42
  %44 = sitofp i32 %32 to double
  %45 = fadd double %44, %43
  %46 = fptosi double %45 to i32
  %47 = sext i32 %.02 to i64
  %48 = getelementptr inbounds [100 x double], [100 x double]* %2, i64 0, i64 %47
  %49 = load double, double* %48, align 8
  %50 = sext i32 %.02 to i64
  %51 = getelementptr inbounds [100 x [100 x double]], [100 x [100 x double]]* %1, i64 0, i64 %50
  %52 = sext i32 %.02 to i64
  %53 = getelementptr inbounds [100 x double], [100 x double]* %51, i64 0, i64 %52
  %54 = load double, double* %53, align 8
  %55 = fmul double %49, %54
  %56 = sitofp i32 %46 to double
  %57 = fadd double %56, %55
  %58 = fptosi double %57 to i32
  br label %59

; <label>:59:                                     ; preds = %18
  %60 = add nsw i32 %.0, 1
  br label %16

; <label>:61:                                     ; preds = %16
  br label %62

; <label>:62:                                     ; preds = %61
  %63 = add nsw i32 %.03, 1
  br label %13

; <label>:64:                                     ; preds = %13
  br label %65

; <label>:65:                                     ; preds = %64
  %66 = add nsw i32 %.02, 1
  br label %10

; <label>:67:                                     ; preds = %10
  %68 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str, i32 0, i32 0), i32 %.01)
  ret i32 0
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i32, i1) #1

declare i32 @printf(i8*, ...) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"}
