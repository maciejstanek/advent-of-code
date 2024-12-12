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

fn main() {
    let inputs = parse();
    let mut sum_safe: i32 = 0;
    for input in inputs {
        let result = input
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
        if !has_zero && has_positive != has_negative {
            sum_safe += 1;
        }
    }
    println!("{}", sum_safe);
}
