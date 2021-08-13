
![Example Image](https://i.imgur.com/dljRSkf.png)
# ANSIFade
**Easily create fade gradiants using ASNI color codes**<br>
Easily create
beautiful looking commandline graphics that can be viewed on any terminal
supporting ANSI.


## Installation<br>
```
git clone https://github.com/armanirodriguez/ansifade
cd ansifade
make
```

## Usage<br>
./ansifade [OPTION...] Color1 Color2<br>
You may enter color sequences as a string or as a RGB sequence<br>

Example: ./ansifade red blue OR ./ansifade red 0,0,255

**Preset Colors**
* red
* blue
* green
* yellow
* lime
* cyan
* pink
* purple
* white
* grey

## Options<br>
  -b, --blacklist=chars : Specify characters to blacklist from coloring. <br>Ex: `./ansifade -b abc red blue #don't color characters a, b or c.`<br><br>
  -w, --whitelist=chars : Whitelist which characters will be colored. <br>Ex: `./ansifade -w abc red blue #only color characters a, b and c`<br><br>
  -o, --output=filename : Redirect output to a file instead of stdout.<br>
