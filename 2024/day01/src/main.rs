use std::io::{stdin, BufRead};

fn parse() -> (Vec<i32>, Vec<i32>) {
    let mut aa = Vec::<i32>::new();
    let mut bb = Vec::<i32>::new();
    for line in stdin().lock().lines() {
        let line = line.unwrap();
        let (a, b) = line.split_at(line.find(' ').unwrap());
        let a = a.trim();
        let b = b.trim();
        aa.push(a.parse::<i32>().unwrap());
        bb.push(b.parse::<i32>().unwrap());
    }
    aa.sort();
    bb.sort();
    (aa, bb)
}

fn main() {
    let (aa, bb) = parse();
    let cc: i32 = aa.iter().zip(bb.iter()).map(|x| i32::abs(x.0 - x.1)).sum();
    println!("{}", cc);
}
