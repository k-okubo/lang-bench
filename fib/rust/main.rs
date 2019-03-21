use std::io;

fn fib(n: i32) -> i32 {
    if n <= 2 {
        1
    } else {
        fib(n - 1) + fib(n - 2)
    }
}

fn main() {
    let mut n = String::new();
    io::stdin().read_line(&mut n).expect("Failed to read");
    let n: i32 = match n.trim().parse() {
        Ok(x) => x,
        Err(_) => 0,
    };

    println!("{}", fib(n));
}
