use std::io::{stdin, BufRead};

fn parse() -> Vec<Vec<i32>> {
    let mut inputs = Vec::<Vec<i32>>::new();
    for line in stdin().lock().lines() {
        inputs.push(
            line.unwrap()
                .trim()
                .split_whitespace()
                .map(|i| i.parse::<i32>())
                .map(Result::unwrap)
                .collect(),
        );
    }
    inputs
}

fn is_safe(sequence: &Vec<i32>) -> bool {
    let result = sequence
        .windows(2)
        .map(|ab| {
            let [a, b] = ab else {
                todo!();
            };
            *b - *a
        })
        .map(|x| if x > 3 || x < -3 { 0 } else { x })
        .collect::<Vec<_>>();

    let has_zero = result.iter().any(|x| *x == 0);
    let has_positive = result.iter().any(|x| *x > 0);
    let has_negative = result.iter().any(|x| *x < 0);

    !has_zero && has_positive != has_negative
}

fn is_safe_with_dampener(sequence: &Vec<i32>) -> bool {
    if is_safe(sequence) {
        return true;
    }
    (0..sequence.len())
        .map(|n| is_safe_except_nth(sequence, n))
        .collect::<Vec<_>>()
        .iter()
        .any(|x| *x)
}

fn is_safe_except_nth(sequence: &Vec<i32>, n: usize) -> bool {
    let new_sequence = [&sequence[0..n], &sequence[n + 1..sequence.len()]].concat();
    is_safe(&new_sequence)
}

fn main() {
    let inputs = parse();
    let sum_safe: i32 = inputs.iter().map(|x| is_safe(x) as i32).sum();
    let sum_safe_with_dampener: i32 = inputs.iter().map(|x| is_safe_with_dampener(x) as i32).sum();
    println!("{}", sum_safe);
    println!("{}", sum_safe_with_dampener);
}
