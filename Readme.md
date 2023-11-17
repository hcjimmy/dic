# dic - a cli dice-calculator for TTRPGs.

- [Motivations](#motivations)
- [Usage](#usage)
- [Install](#install)
- [Todo](#todo)
- [Name](#name)
- [License](#license)

## Motivations

As a DM, I was disappointed with the open-source dice-rolling applications I had.

After thinking about it a while, I eventually decided it'd be a good idea to make one as a calculator. I mean, the syntax already exists;
if you ask pretty much any D&D player _"gimme a 4d8-3"_, they'll know you mean _roll an 8-sided die 4 times, and subtract by 3_.

So it was pretty much a matter of implementing it then adding extra features. And as a bonus you can use it as a normal calculator.

Admittedly though, I didn't look very hard at other options. Especially at desktop ([this app seems very similar to mine](https://github.com/dice-roller/cli)), but I don't regret making this.

## Usage

On a terminal, enter `dic` followed by the calculation. Just put dice in place of numbers in the syntax of `<repeats>d<sides>`.

Here're some examples:

```
$ dic d20
4
$ dic 2d10 + 5
2+1+5 = 8
$ dic d10 --repeat 3   # Repeat 3 times
8
4
4
$ dic d20+4 -r d6      # Repeat d6 times
9+4 = 13
7+4 = 11
7+4 = 11
15+4 = 19
```

Here's the full help:

```
dic - a dice calculator for TTRPGs.

Usage:
  dic <dice-expression>
  dic [options] <args> <dice-expression>

Options:
  -h    --help            Show this help message.
  -r    --repeat <n>      Repeat the calculation n times.
  -A    --advantage       Roll twice and take the higher (show the lower greyed-out).
  -D    --disadvantage    Roll twice and take the lower (show the higher greyed-out).
  -c    --collapse        Don't show all the dice-rolls if the die operated by not +- (default).
                          E.g. dic 2*3d4 will give 2*7 = 14  instead of  2*(2+2+3) = 14.
  -x    --expand          Always show all the dice rolls (opposite of --collpase).
  -q    --quiet           Don't show the calculation, just the result.
  -H    --hide-grey       Hide the greyed-out output of --disadvantage or --advantage.
  -Q    --really-quiet    Don't print the calculation or the greyed-out output.
                          Equivalent to giving both -H and -q.

Examples:
  dic 2d4                 Roll a 4-sided die 2 times.
  dic d20+5               Roll a 20-sided die and add 5.
  dic d20+5 -r 3          Calculate d20+5, 3 times, and print the results seperately.
  dic -r d4  d20 - d6     Calculate d20-d6, 1d4 times.

    Allowed operators: +-/*%()
```

### Limitations

- Running `dic 2 * 3` will result in bash expanding `*` to all the files in the current directory... admittedly the output can be pretty funny though...

- Running `dic 3(4)` on bash will result in ``bash: syntax error near unexpected token `('``...

Note: these problems have nothing to do with dic, but bash. This is because bash sometimes modifies the input written to it (as defined by it's syntax) before passing it to the program. This is usually good (it's convenient).

I will soon add support for other parenthesis, until then you can do `dic '3(4)'`.

## Install (build)

##### [Linux](#linux), [Windows](#windows)

I've only tested this program on Linux, but it *should* work on other platforms.

### Linux

- Install `cmake`, `gcc` and `make` (should be at your distro's repository).
- Clone this repo (or download the code above).
- Enter the `build` directory inside it (in a terminal).
- Run:

```
cmake ../src -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cmake --install .
```

The last command may require root. If like me, you prefer local installations, replace it with:

```
cmake --install . --prefix ~/.local
```

This will put dic in ~/.local/bin instead, so you'll need to add ~/.local/bin to your PATH if it's not there already.

### Windows

- It'll be necessary to put the stuff you'll install in a folder in your PATH variable. [See guide](https://stackoverflow.com/questions/44272416/how-to-add-a-folder-to-path-environment-variable-in-windows-10-with-screensho).

  - Essentially, the PATH variable is a list of folders for which programs inside them are made available to the command prompt. Some installers might add it by themselves, but I don't ship one.

#### Prerequisites

Install `cmake`, `clang` and `ninja`, these are tools necessary to build this program.

It'll probably be simpler to install them through a package manager like [`scoop`](https://scoop.sh/) or [`chocolate`](https://chocolatey.org/install#individual); then you'll just need to run `choco install cmake llvm ninja` or `scoop install main/cmake main/llvm main/ninja` and it'll install and add them to your PATH.

(I don't have windows, but the above commands *should* work).

#### Steps
 
- Clone this repository (or just download the code above).
- Enter the `build` directory inside it (in cmd/powershell, you can shift+right click in windows file-manager IIRC).
- Run:

```
cmake ../src -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

This should make the binary `dic.exe` (either in the current directory or in the `Release` folder).

Now outside windows, I know running `cmake --install .` should do the trick (and on windows it'll probably install it in Program Files or something [this is a cmake feature]); but this is a CLI program (command line interface), you'll need to add it to your PATH.

So just follow the guide above to make a folder and add it to your PATH, then put `dic.exe` inside it.

---

Enjoy.

## Todo

- [ ] Support other kinds of parenthesis, so bash will be happy `[]{}`.
- [ ] Provide binaries (and maybe an installer through cmake?), ideally built on github's servers.
- [ ] Interactive mode?

## Name

I know what you're thinking.

For the record, originally dic was _supposed_ to be pronounced diec, short for _diculator_, as in dice-calculator.

However _you_ pronounce it though, is your business; much like it is your business how you pronounce gif.

# License

This project is licensed under GPLv3.
