<?hh // decl /* -*- php -*- */
/**
 * Copyright (c) 2014, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the 'hack' directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 */

class RecursiveIteratorIterator<T> implements Iterator<T>, OuterIterator<T> {
  const int LEAVES_ONLY = 0;
  const int SELF_FIRST = 1;
  const int CHILD_FIRST = 2;
  const int CATCH_GET_CHILD = 16;
  public function __construct(Traversable<T> $iterator, $mode = null, $flags = null) {}
  public function beginChildren() {}
  public function beginIteration() {}
  public function callGetChildren() {}
  public function callHasChildren() {}
  public function current() {}
  public function endChildren() {}
  public function endIteration() {}
  public function getDepth() {}
  public function getInnerIterator() {}
  public function getMaxDepth() {}
  public function getSubIterator($level = null) {}
  public function key() {}
  public function next() {}
  public function nextElement() {}
  public function rewind() {}
  public function setMaxDepth($max_depth = null) {}
  public function valid() {}
}

class IteratorIterator<T> implements Iterator<T>, OuterIterator<T> {
  public function __construct(Traversable $iterator) {}
  public function current() {}
  public function getInnerIterator() {}
  public function key() {}
  public function next() {}
  public function rewind() {}
  public function valid() {}
}

abstract class FilterIterator<T> extends IteratorIterator<T> {
  abstract public function accept();
}

abstract class RecursiveFilterIterator<T> extends FilterIterator<T> implements RecursiveIterator<T> {
  public function getChildren() {}
  public function hasChildren() {}
}

class CallbackFilterIterator<T> extends FilterIterator<T> {
  public function accept() {}
}

class RecursiveCallbackFilterIterator<T> extends CallbackFilterIterator<T> implements RecursiveIterator<T> {
  public function getChildren() {}
  public function hasChildren() {}
}

class ParentIterator<T> extends RecursiveFilterIterator<T> {
  public function accept() {}
}

class LimitIterator<T> extends IteratorIterator<T> {
  public function getPosition() {}
  public function seek($position) {}
}

class CachingIterator<Tk, Tv> extends IteratorIterator<Tv> implements ArrayAccess<Tk, Tv>, Countable {
  const int CALL_TOSTRING = 1;
  const int CATCH_GET_CHILD = 16;
  const int TOSTRING_USE_KEY = 2;
  const int TOSTRING_USE_CURRENT = 4;
  const int TOSTRING_USE_INNER = 8;
  const int FULL_CACHE = 256;
  public function __toString() {}
  public function count() {}
  public function getCache() {}
  public function getFlags() {}
  public function hasNext() {}
  public function offsetExists($index) {}
  public function offsetGet($index) {}
  public function offsetSet($index, $newval) {}
  public function offsetUnset($index) {}
  public function setFlags($flags) {}
}

class RecursiveCachingIterator<Tk, Tv> extends CachingIterator<Tk, Tv> implements RecursiveIterator<Tv> {
  public function getChildren() {}
  public function hasChildren() {}
}

class NoRewindIterator<T> extends IteratorIterator<T> {
}

class AppendIterator<T> extends IteratorIterator<T> {
  public function append(Iterator $iterator) {}
  public function getArrayIterator() {}
  public function getIteratorIndex() {}
}

class InfiniteIterator<T> extends IteratorIterator<T> {
}

class RegexIterator<T> extends FilterIterator<T> {
  const int USE_KEY = 1;
  const int INVERT_MATCH = 2;
  const int MATCH = 0;
  const int GET_MATCH = 1;
  const int ALL_MATCHES = 2;
  const int SPLIT = 3;
  const int REPLACE = 4;
  public $replacement;
  public function accept() {}
  public function getFlags() {}
  public function getMode() {}
  public function getPregFlags() {}
  public function getRegex() {}
  public function setFlags($flags) {}
  public function setMode($mode) {}
  public function setPregFlags($preg_flags) {}
}

class RecursiveRegexIterator<T> extends RegexIterator<T> implements RecursiveIterator<T> {
  public function getChildren() {}
  public function hasChildren() {}
}

class EmptyIterator<T> implements Iterator<T> {
  public function current() {}
  public function key() {}
  public function next() {}
  public function rewind() {}
  public function valid() {}
}

class RecursiveTreeIterator<T> extends RecursiveIteratorIterator<T> {
  const int BYPASS_CURRENT = 4;
  const int BYPASS_KEY = 8;
  const int PREFIX_LEFT = 0;
  const int PREFIX_MID_HAS_NEXT = 1;
  const int PREFIX_MID_LAST = 2;
  const int PREFIX_END_HAS_NEXT = 3;
  const int PREFIX_END_LAST = 4;
  const int PREFIX_RIGHT = 5;
  public function getEntry() {}
  public function getPostfix() {}
  public function getPrefix() {}
  public function setPostfix() {}
  public function setPrefixPart($part, $value) {}
}

class ArrayObject<Tk, Tv> implements IteratorAggregate<Tv>, ArrayAccess<Tk, Tv>, Serializable, Countable {
  const int STD_PROP_LIST = 1;
  const int ARRAY_AS_PROPS = 2;
  public function __construct($array) {}
  public function append($value) {}
  public function asort() {}
  public function count() {}
  public function exchangeArray($array) {}
  public function getArrayCopy() {}
  public function getFlags() {}
  public function getIterator() {}
  public function getIteratorClass() {}
  public function ksort() {}
  public function natcasesort() {}
  public function natsort() {}
  public function offsetExists($index) {}
  public function offsetGet($index) {}
  public function offsetSet($index, $newval) {}
  public function offsetUnset($index) {}
  public function serialize() {}
  public function setFlags($flags) {}
  public function setIteratorClass($iteratorClass) {}
  public function uasort($cmp_function) {}
  public function uksort($cmp_function) {}
  public function unserialize($serialized) {}
}

class RecursiveArrayIterator<T> extends ArrayIterator<T> implements RecursiveIterator<T> {
  const int CHILD_ARRAYS_ONLY = 4;
  public function getChildren() {}
  public function hasChildren() {}
}

class FilesystemIterator extends DirectoryIterator {
  const int CURRENT_MODE_MASK = 240;
  const int CURRENT_AS_PATHNAME = 32;
  const int CURRENT_AS_FILEINFO = 0;
  const int CURRENT_AS_SELF = 16;
  const int KEY_MODE_MASK = 3840;
  const int KEY_AS_PATHNAME = 0;
  const int FOLLOW_SYMLINKS = 512;
  const int KEY_AS_FILENAME = 256;
  const int NEW_CURRENT_AND_KEY = 256;
  const int OTHER_MODE_MASK = 12288;
  const int SKIP_DOTS = 4096;
  const int UNIX_PATHS = 8192;
  public function __construct($path, $flags) {}
  public function getFlags() {}
  public function setFlags($flags = null) {}
}

class RecursiveDirectoryIterator<T> extends FilesystemIterator implements RecursiveIterator<DirectoryIterator> {
  public function __construct($path, $flags) {}
  public function getChildren() {}
  public function getSubPath() {}
  public function getSubPathname() {}
  public function hasChildren($allow_links = null) {}
}

class GlobIterator extends FilesystemIterator implements Countable {
  public function count() {}
}

class SplStack<T> extends SplDoublyLinkedList<T> {
}

abstract class SplHeap<T> implements Iterator<T>, Countable {
  abstract public function compare();
  public function count() {}
  public function current() {}
  public function extract() {}
  public function insert($value) {}
  public function isEmpty() {}
  public function key() {}
  public function next() {}
  public function recoverFromCorruption() {}
  public function rewind() {}
  public function top() {}
  public function valid() {}
}

class SplMinHeap<T> extends SplHeap<T> {
  public function compare() {}
}

class SplMaxHeap<T> extends SplHeap<T> {
  public function compare() {}
}

class SplPriorityQueue<T> implements Iterator<T>, Countable {
  const int EXTR_BOTH = 3;
  const int EXTR_PRIORITY = 2;
  const int EXTR_DATA = 1;
  public function compare($a, $b) {}
  public function count() {}
  public function current() {}
  public function extract() {}
  public function insert($value, $priority) {}
  public function isEmpty() {}
  public function key() {}
  public function next() {}
  public function recoverFromCorruption() {}
  public function rewind() {}
  public function setExtractFlags($flags) {}
  public function top() {}
  public function valid() {}
}

class SplFixedArray<Tk, Tv> implements Iterator<Tv>, ArrayAccess<Tk, Tv>, Countable {
  public function __construct($size = null) {}
  public function __wakeup() {}
  public function count() {}
  public function current() {}
  public static function fromArray($data, $save_indexes = null) {}
  public function getSize() {}
  public function key() {}
  public function next() {}
  public function offsetExists($index) {}
  public function offsetGet($index) {}
  public function offsetSet($index, $newval) {}
  public function offsetUnset($index) {}
  public function rewind() {}
  public function setSize($value) {}
  public function toArray() {}
  public function valid() {}
}

interface SplObserver {
  public function update(SplSubject $SplSubject) {}
}

interface SplSubject {
  public function attach(SplObserver $SplObserver) {}
  public function detach(SplObserver $SplObserver) {}
  public function notify() {}
}

class SplObjectStorage<Tk, Tv> implements Countable, Iterator<Tv>, Serializable, ArrayAccess<Tk, Tv> {
  public function addAll($object) {}
  public function attach($object, $inf = null) {}
  public function contains($object) {}
  public function count() {}
  public function current() {}
  public function detach($object) {}
  public function getHash($object) {}
  public function getInfo() {}
  public function key() {}
  public function next() {}
  public function offsetExists($object) {}
  public function offsetGet($object) {}
  public function offsetSet($object, $inf = null) {}
  public function offsetUnset($object) {}
  public function removeAll($object) {}
  public function removeAllExcept($object) {}
  public function rewind() {}
  public function serialize() {}
  public function setInfo($info) {}
  public function unserialize($serialized) {}
  public function valid() {}
}

class MultipleIterator<T> implements Iterator<T> {
  const int MIT_NEED_ANY = 0;
  const int MIT_NEED_ALL = 1;
  const int MIT_KEYS_NUMERIC = 0;
  const int MIT_KEYS_ASSOC = 2;
  public function __construct($flags) {}
  public function attachIterator(Iterator $iterator, $infos = null) {}
  public function containsIterator(Iterator $iterator) {}
  public function countIterators() {}
  public function current() {}
  public function detachIterator(Iterator $iterator) {}
  public function getFlags() {}
  public function key() {}
  public function next() {}
  public function rewind() {}
  public function setFlags($flags) {}
  public function valid() {}
}

class SplType {
  public function __construct($initial_value, $strict) {}
}

class SplInt extends SplType {
}

class SplFloat extends SplType {
}

class SplString extends SplType {
}

class SplEnum extends SplType {
  public function __construct($initial_value, $strict) {}
  public function getConstList($include_default = false) {}
}

class SplBool extends SplEnum {
}
