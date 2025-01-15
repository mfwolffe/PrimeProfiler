# What is this?
This is just some binary instrumentation of a few naive primality algorithms.

## why?
In winter of 2023 I mentioned to Dr. Lam (JMU) I was interested in tinkering with performance analysis and instrumentation on some code I wrote for his course, and he recommended I look into intel PIN.

With all the very entertaining overengineering (not really anything special, just tons of preprocessor macros, lookup tables w/ function ptrs, etc.) of later PAs for his course I did I'm surprised I opted for the code I did?
so what code? It came from my CS 261 p0 submission, which is sort of a primer to C and has students writing a number of somewhat-rudimentary functions. One of them was a primality test, and there's this 6k +/- 1 necessity for primes >3 I had seen which can be exploited in algorithms to significantly cut down the number of comparisons needed to get a passing result. Other little optimizations include: checking only up to square root of the prime candidate, and then further tweak that by making the comparison of the current divisor squared against the actual candidate to avoid slowdowns I assume would be introduced by the sqrt() function, but I should look into that. The algorithm with comments is in `src/main.c`

So I was curious to see just how much faster that algorithm was than a brute force approach, but given that, at least asymptotically, the two approaches are quite similar, I asked Dr. Lam if there's any other instrumentation that could still be interesting. He mentioned that on modern hardware with compiler optimizations, it might not actually be that the *seemingly-less-naive* algorithm is markedly more efficient, and then some gears churned and then he mentioned intel pin. Then came his challenge for me to do it, the reward, of course self-edification.

Then I did really nothing with it for a long time - reason being from a *very cursory* glance it seemed like getting pin setup would not be possible with my bizarre schedule. Here's to changing that, and ...self-edification!


#### long term goals for this
- use this as an excuse to learn basics of low-level languages, then instrument those binaries

<hr>

### matt's devlog/notes
<p style="font-size: 0.7rem">I'm tryna be better about documenting my dev/build processes. shhhhh</p>

- Makefiles for specific binaries are populated by my `script/populate-makefiles.sh` script and template makefile w/ canary.
  - this seemed faster to me than manually editing each makefile
- I opted to just use the [`pin`](https://aur.archlinux.org/packages/pin) package on the AUR over whatever intel's website has (maybe I should've checked lol).

> this is nothing to do with this project really I just got excited when I noticed it/how I've never noticed it before. 
> I was ssh'd into stu to find my original less naive C implementation, started to run a `micro` command, forgetting `micro` is not on Stu
> and I got the following error:
> ```bash
> -bash: micro: command not found
> # wait a minute. what is that.
> # -bash: mic...
> # -bash: ...
> # -b
> # -
> # is it there because this is a login shell?
> # I know it is a login shell, I even double checked
> # I just cannot recall if that is always there????
> ```
>

#### todos?
- I don't know if I want to 'ship' the makefiles with the most pedantic compiler flags set. I don't understand all of it but, people smarter than me get up in arms about -Werror and introducing dependencies on specific toolchains.
  - Obviously, compile as pedantic as possible, just consider removal at some point, or I'm not sure what. 
  - Definitely Maybe people already have thought about this a lot and there's already a canonical approach I just need to learn about 
- compare how parameter types can impact?
  - the original implementation I submitted passed in an `int` (because the prototype called for it and if we modified header files that would've been a zero)
    - I immediately rewrote w/ unsigned int.. we got about 2 billion more numbers, folks
  - theoretically passing larger int types could be less performant than basic int?
  - I honestly don't know the number of digits at which it makes more sense to use a probabilistic primality test, so it could be moot, but it also could be interesting to see how they fare & compare on very large primes.
- maybe a different approach to the makefiles? it's kiinda messy.
  - I could write a small script that repeatedly modifies a single makefile in-place, likely just with `sed`, and then invokes make each time (lol)? maybe?
    - remember the wise words of Dr. Kirkpatrick though, *memory is essentially free*. 


#### things to be aware of
- with higher levels of compiler optimizations esp. `0fast`, subtle bugs can be introduced if not programming defensively.
- code-to-instrument need not be in C++ (it's a *binary* instrumentation tool after all...)
- artifacts in perf metrics from having repeated calls with small/large inputs?
  - idk if cache effects could cause a spurious dip or blip

