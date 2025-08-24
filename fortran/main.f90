! Main Fortran program for prime testing comparison
! Matt Wolffe, James Madison University, 2025
! Direct translation of C main.c test harness

program prime_test_fortran
    implicit none
    
    ! Function interfaces
    interface
        function naive_prime(n) result(is_prime)
            integer, intent(in) :: n
            logical :: is_prime
        end function naive_prime
        
        function naive_prime_squares(n) result(is_prime)
            integer, intent(in) :: n
            logical :: is_prime
        end function naive_prime_squares
        
        function less_naive_prime(n) result(is_prime)
            integer, intent(in) :: n
            logical :: is_prime
        end function less_naive_prime
    end interface
    
    ! Test case structure
    type :: test_case_t
        integer :: n
        logical :: expected
    end type test_case_t
    
    ! Test cases (identical to C version)
    type(test_case_t), parameter :: test_cases(27) = [ &
        test_case_t(2, .true.), test_case_t(3, .true.), test_case_t(11, .true.), &
        test_case_t(47, .true.), test_case_t(97, .true.), test_case_t(151, .true.), &
        test_case_t(19, .true.), test_case_t(89, .true.), test_case_t(93, .false.), &
        test_case_t(33, .false.), test_case_t(12, .false.), test_case_t(133, .false.), &
        test_case_t(5927, .true.), test_case_t(6007, .true.), test_case_t(15013, .true.), &
        test_case_t(3259, .false.), test_case_t(5773, .false.), test_case_t(8411, .false.), &
        test_case_t(150113, .false.), test_case_t(87943, .true.), test_case_t(12289, .true.), &
        test_case_t(25111, .true.), test_case_t(41893, .true.), test_case_t(52297, .true.), &
        test_case_t(8832, .false.), test_case_t(9376, .false.), test_case_t(8468, .false.) ]
    
    integer :: i
    logical :: result
    character(len=5) :: result_str, expected_str
    
    ! Test all algorithms with all cases
    print *, 'Fortran Prime Algorithm Testing'
    print *, '=============================='
    print *
    
    ! Test naive_prime
    print *, 'Testing naive_prime:'
    do i = 1, size(test_cases)
        result = naive_prime(test_cases(i)%n)
        if (result) then
            result_str = 'true'
        else
            result_str = 'false'
        end if
        if (test_cases(i)%expected) then
            expected_str = 'true'
        else
            expected_str = 'false'
        end if
        print '(a,i0,a,a,a,a,a)', 'naive_prime(', test_cases(i)%n, ') = ', &
              trim(result_str), ' (expected ', trim(expected_str), ')'
    end do
    print *
    
    ! Test naive_prime_squares
    print *, 'Testing naive_prime_squares:'
    do i = 1, size(test_cases)
        result = naive_prime_squares(test_cases(i)%n)
        if (result) then
            result_str = 'true'
        else
            result_str = 'false'
        end if
        if (test_cases(i)%expected) then
            expected_str = 'true'
        else
            expected_str = 'false'
        end if
        print '(a,i0,a,a,a,a,a)', 'naive_prime_squares(', test_cases(i)%n, ') = ', &
              trim(result_str), ' (expected ', trim(expected_str), ')'
    end do
    print *
    
    ! Test less_naive_prime
    print *, 'Testing less_naive_prime:'
    do i = 1, size(test_cases)
        result = less_naive_prime(test_cases(i)%n)
        if (result) then
            result_str = 'true'
        else
            result_str = 'false'
        end if
        if (test_cases(i)%expected) then
            expected_str = 'true'
        else
            expected_str = 'false'
        end if
        print '(a,i0,a,a,a,a,a)', 'less_naive_prime(', test_cases(i)%n, ') = ', &
              trim(result_str), ' (expected ', trim(expected_str), ')'
    end do
    
end program prime_test_fortran