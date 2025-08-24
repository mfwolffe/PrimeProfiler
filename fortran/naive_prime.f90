! Naive prime testing algorithm - Fortran implementation
! Matt Wolffe, James Madison University, 2025
! Direct translation of C naive_prime algorithm

function naive_prime(n) result(is_prime)
    implicit none
    integer, intent(in) :: n
    logical :: is_prime
    integer :: i
    
    ! Handle edge cases
    if (n <= 1) then
        is_prime = .false.
        return
    end if
    
    ! Test all divisors from 2 to n-1
    do i = 2, n - 1
        if (mod(n, i) == 0) then
            is_prime = .false.
            return
        end if
    end do
    
    is_prime = .true.
end function naive_prime