# Global Functions #

The following global functions are available with the Debug package. [Learn more information about these functions 
in the dumping variables chapter](debugging.md#dumping-variables). 

<div class="notice is-info">
    If error reporting is off, these functions will not output anything.
 </div>

### debug(`... $args`): `void` ###

Output the value of a variable, or multiple variables.

```hack
debug($foo, $bar);
```

### dump(`... $args`): `void` ###

Output the value of a variable, or multiple variables, and include type information.

```hack
dump($foo, $bar);
```

### backtrace(`?Exception $e`): `void` ###

Output the back trace of the script, or the stack trace of an exception.

```hack
backtrace();
backtrace($e);
```

### inspect(`Exception $e`): `void` ###

Output the stack trace, file path, line number, and metadata of an exception.

```hack
inspect($e);
```

### export(`mixed $var`): `void` ###

Output the variable into a PHP/Hack usable format.

```hack
export($baz);
```
