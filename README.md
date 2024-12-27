# cll

`cll` is a an attempt at a linked list data structure in C. I purposly used a model that I thought that
the Rust borrow checker would approve of.

## Requirements
`clang` C compiler. Will probably compile with other compilers, but I have not checked them.

## Installation

1. Clone the git repository
1. Run build.sh.

   ```sh
   ./build.sh
   ```

## Issues
1. Because of the way we find unoccupied slots available for new nodes, the
   current implementation of `ll_add_first` is O(n) instead of O(1). 

## License

[MIT](LICENSE) © [gmrowe](https://github.com/gmrowe).

