! Less naive prime testing algorithm - Fortran implementation  
! Matt Wolffe, James Madison University, 2025
! Direct translation of C less_naive_prime algorithm with optimizations

function less_naive_prime(n) result(is_prime)
    implicit none
    integer, intent(in) :: n
    logical :: is_prime
    integer :: i, sqrt_n
    
    ! Handle edge cases
    if (n <= 1) then
        is_prime = .false.
        return
    end if
    
    if (n <= 3) then
        is_prime = .true.
        return
    end if
    
    ! Check if even or divisible by 3
    if (mod(n, 2) == 0 .or. mod(n, 3) == 0) then
        is_prime = .false.
        return
    end if
    
    ! Calculate integer square root
    sqrt_n = int(sqrt(real(n)))
    
    ! Check divisors of form 6k ± 1
    i = 5
    do while (i <= sqrt_n)
        if (mod(n, i) == 0 .or. mod(n, i + 2) == 0) then
            is_prime = .false.
            return
        end if
        i = i + 6
    end do
    
    is_prime = .true.
end function less_naive_prime