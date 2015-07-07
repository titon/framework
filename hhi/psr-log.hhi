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

namespace Psr\Log;

class LogLevel {
    const string EMERGENCY = 'emergency';
    const string ALERT = 'alert';
    const string CRITICAL = 'critical';
    const string ERROR = 'error';
    const string WARNING = 'warning';
    const string NOTICE = 'notice';
    const string INFO = 'info';
    const string DEBUG = 'debug';
}

interface LoggerInterface {
    public function emergency(string $message, array<string, mixed> $context = []): void;
    public function alert(string $message, array<string, mixed> $context = []): void;
    public function critical(string $message, array<string, mixed> $context = []): void;
    public function error(string $message, array<string, mixed> $context = []): void;
    public function warning(string $message, array<string, mixed> $context = []): void;
    public function notice(string $message, array<string, mixed> $context = []): void;
    public function info(string $message, array<string, mixed> $context = []): void;
    public function debug(string $message, array<string, mixed> $context = []): void;
    public function log(mixed $level, string $message, array<string, mixed> $context = []): void;
}

abstract class AbstractLogger implements LoggerInterface {
    public function emergency(string $message, array<string, mixed> $context = []): void {}
    public function alert(string $message, array<string, mixed> $context = []): void {}
    public function critical(string $message, array<string, mixed> $context = []): void {}
    public function error(string $message, array<string, mixed> $context = []): void {}
    public function warning(string $message, array<string, mixed> $context = []): void {}
    public function notice(string $message, array<string, mixed> $context = []): void {}
    public function info(string $message, array<string, mixed> $context = []): void {}
    public function debug(string $message, array<string, mixed> $context = []): void {}
}
