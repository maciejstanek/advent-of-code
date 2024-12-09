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
    (aa, bb)
}

fn main() {
    let (aa, bb) = parse();
    let mut a_sorted = aa.clone();
    let mut b_sorted = bb.clone();
    a_sorted.sort();
    b_sorted.sort();
    let cc: i32 = a_sorted
        .iter()
        .zip(b_sorted.iter())
        .map(|x| i32::abs(x.0 - x.1))
        .sum();
    println!("{}", cc);
    let mut aa_counts = Vec::<usize>::new();
    for a in aa {
        let partial: usize = bb.iter().filter(|x| **x == a).count();
        aa_counts.push(partial * (a as usize));
    }
    println!("{}", aa_counts.iter().sum::<usize>());
}
