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

class finfo {
  public function __construct($options = null, $arg = null);
  public function buffer($string, $options = null, $context = null);
  public function file($filename, $options = null, $context = null);
  public function set_flags($options);
}
