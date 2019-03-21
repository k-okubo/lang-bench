use std::io;

struct Node {
    l: Option<Box<Node>>,
    r: Option<Box<Node>>,
}

fn make_tree(num_alloc: &mut i32, depth: i32) -> Option<Box<Node>> {
    if depth <= 0 {
        None
    } else {
        *num_alloc += 1;
        Some(Box::new(Node {
            l: make_tree(num_alloc, depth - 1),
            r: make_tree(num_alloc, depth - 1)
        }))
    }
}

fn main() {
    let mut depth = String::new();
    io::stdin().read_line(&mut depth).expect("Failed to read");
    let depth: i32 = match depth.trim().parse() {
        Ok(x) => x,
        Err(_) => 0,
    };

    let mut times = String::new();
    io::stdin().read_line(&mut times).expect("Failed to read");
    let times: i32 = match times.trim().parse() {
        Ok(x) => x,
        Err(_) => 0,
    };

    let mut num_alloc = 0;
    let mut i = 0;
    while i < times {
        let mut _last_tree: Option<Box<Node>> = None;

        let mut d = 1;
        while d <= depth {
            let tree = make_tree(&mut num_alloc, d);
            _last_tree = tree;
            d = d + 1;
        }
        i = i + 1;
    }

    println!("{}", num_alloc);
}
