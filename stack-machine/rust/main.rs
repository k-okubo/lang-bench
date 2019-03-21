use std::io;
use std::rc::Rc;

#[derive(Clone)]
enum Value {
    Nil,
    Integer(i32),
    Object(Rc<Object>),
    Function(usize),
    Pointer(usize),
}

struct Object {
    values: Vec<Value>,
}

enum Instruction {
    Dec,
    Push(Value),
    LoadA(usize),
    New(usize),
    JNZ(usize),
    Call(usize),
    Ret,
    Exit,
}

fn debug_dump(inst: &Instruction, stack: &Vec<Value>, sp: usize) {
    println!("");

    match inst {
        Instruction::Dec       => { println!("Dec") },
        Instruction::Push(_)   => { println!("Push") },
        Instruction::LoadA(x)  => { println!("LoadA {}", x) },
        Instruction::New(x)    => { println!("New {}", x) },
        Instruction::JNZ(x)    => { println!("JNZ {}", x) },
        Instruction::Call(x)   => { println!("Call {}", x) },
        Instruction::Ret       => { println!("Ret") },
        Instruction::Exit      => { println!("Exit") },
    }

    let mut p = sp;
    while p > 0 {
        match stack[p - 1] {
            Value::Nil         => { println!("  nil") },
            Value::Integer(x)  => { println!("  int {}", x) },
            Value::Object(_)   => { println!("  obj") },
            Value::Function(x) => { println!("  func {}", x) },
            Value::Pointer(x)  => { println!("  ptr {}", x) },
        }
        p -= 1;
    }
}

fn run_stack_machine(num_alloc: &mut i32, depth: i32) {
    let program = [
        /*  0 */ Instruction::Push(Value::Integer(depth)),
        /*  1 */ Instruction::Call(3),
        /*  2 */ Instruction::Exit,

        // make_tree
        /*  3 */ Instruction::LoadA(0),  // if depth == 0
        /*  4 */ Instruction::JNZ(7),
        /*  5 */ Instruction::Push(Value::Nil),
        /*  6 */ Instruction::Ret,

        /*  7 */ Instruction::LoadA(0),  // make_tree(depth - 1)
        /*  8 */ Instruction::Dec,
        /*  9 */ Instruction::Call(3),

        /* 10 */ Instruction::LoadA(0),  // make_tree(depth - 1)
        /* 11 */ Instruction::Dec,
        /* 12 */ Instruction::Call(3),

        /* 13 */ Instruction::New(2),    // new
        /* 14 */ Instruction::Ret,
    ];

    let mut stack = vec![Value::Nil; 4096];

    let mut sp = 0;
    let mut fp = 0;
    let mut pc = 0;
    let mut run = true;

    loop {
        if !run {
            break;
        }

        // debug_dump(&program[pc], &stack, sp);

        match program[pc] {
            Instruction::Dec => {
                match stack[sp - 1] {
                    Value::Integer(x) => { stack[sp - 1] = Value::Integer(x - 1) },
                    _ => { panic!("data type missmatch") },
                }
                pc += 1;
            },
            Instruction::Push(ref x) => {
                stack[sp] = x.clone();
                sp += 1;
                pc += 1;
            },
            Instruction::LoadA(i) => {
                stack[sp] = stack[fp + i].clone();
                sp += 1;
                pc += 1;
            },
            Instruction::New(size) => {
                *num_alloc += 1;

                let mut obj = Object { values: vec![Value::Nil; size] };
                for i in 0..size {
                    obj.values[i] = stack[sp - size + i].clone();
                }

                sp -= size;
                stack[sp] = Value::Object(Rc::new(obj));
                sp += 1;
                pc += 1;
            },
            Instruction::JNZ(to) => {
                sp -= 1;

                match stack[sp] {
                    Value::Integer(x) => {
                        if x != 0 {
                            pc = to;
                        } else {
                            pc += 1;
                        }
                    },
                    _ => { panic!("data type missmatch") },
                }
            },
            Instruction::Call(to) => {
                let num_args = 1;

                stack[sp + 0] = Value::Function(pc);
                stack[sp + 1] = Value::Pointer(fp);
                sp += 2;

                pc = to;
                fp = sp - 2 - num_args;
            },
            Instruction::Ret => {
                let retval = stack[sp - 1].clone();
                let next_sp = fp;

                match stack[sp - 2] {
                    Value::Pointer(x) => { fp = x },
                    _ => { panic!("data type missmatch") },
                }
                match stack[sp - 3] {
                    Value::Function(x) => { pc = x + 1 },
                    _ => { panic!("data type missmatch") },
                }

                sp = next_sp;
                stack[sp] = retval;
                sp += 1;
            },
            Instruction::Exit => {
                run = false;
            },
        }
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

    for _i in 0..times {
        for d in 1..(depth + 1) {
            run_stack_machine(&mut num_alloc, d);
        }
    }

    println!("{}", num_alloc);
}
