! Naive prime with square root optimization - Fortran implementation
! Matt Wolffe, James Madison University, 2025
! Direct translation of C naive_prime_squares algorithm

function naive_prime_squares(n) result(is_prime)
    implicit none
    integer, intent(in) :: n
    logical :: is_prime
    integer :: i, sqrt_n
    
    ! Handle edge cases
    if (n <= 1) then
        is_prime = .false.
        return
    end if
    
    if (n == 2) then
        is_prime = .true.
        return
    end if
    
    ! Calculate integer square root
    sqrt_n = int(sqrt(real(n)))
    
    ! Test divisors from 2 to sqrt(n)
    do i = 2, sqrt_n
        if (mod(n, i) == 0) then
            is_prime = .false.
            return
        end if
    end do
    
    is_prime = .true.
end function naive_prime_squares