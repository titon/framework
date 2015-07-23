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

namespace Psr\Http\Message;

interface MessageInterface {
  public function getBody(): StreamInterface;
  public function getHeader(string $name): array<string>;
  public function getHeaderLine(string $name): string;
  public function getHeaders(): array<string, array<string>>;
  public function getProtocolVersion(): string;
  public function hasHeader(string $name): bool;
  public function withAddedHeader(string $name, mixed $value): this;
  public function withBody(StreamInterface $body): this;
  public function withHeader(string $name, mixed $value): this;
  public function withProtocolVersion(string $version): this;
  public function withoutHeader(string $name): this;
}

interface RequestInterface extends MessageInterface {
  public function getMethod(): string;
  public function getRequestTarget(): string;
  public function getUri(): UriInterface;
  public function withMethod(string $method): this;
  public function withRequestTarget(string $requestTarget): this;
  public function withUri(UriInterface $uri, bool $preserveHost = false): this;
}

interface ResponseInterface extends MessageInterface {
  public function getReasonPhrase(): string;
  public function getStatusCode(): int;
  public function withStatus(int $code, string $reasonPhrase = ""): this;
}

interface ServerRequestInterface extends RequestInterface {
  public function getAttribute(string $name, mixed $default = null): mixed;
  public function getAttributes(): array<string, mixed>;
  public function getCookieParams(): array<string, mixed>;
  public function getParsedBody(): mixed;
  public function getQueryParams(): array<string, mixed>;
  public function getServerParams(): array<string, mixed>;
  public function getUploadedFiles(): array<UploadedFileInterface>;
  public function withAttribute(string $name, mixed $value): this;
  public function withCookieParams(array<string, mixed> $cookies): this;
  public function withParsedBody(mixed $data): this;
  public function withQueryParams(array<string, mixed> $query): this;
  public function withUploadedFiles(array<UploadedFileInterface> $uploadedFiles): this;
  public function withoutAttribute(string $name): this;
}

interface StreamInterface {
  public function __toString(): string;
  public function close(): void;
  public function detach(): ?resource;
  public function eof(): bool;
  public function getContents(): string;
  public function getMetadata(string $key = ''): mixed;
  public function getSize(): int;
  public function isReadable(): bool;
  public function isSeekable(): bool;
  public function isWritable(): bool;
  public function read(int $length): string;
  public function rewind(): bool;
  public function seek(int $offset, int $whence = SEEK_SET): bool;
  public function tell(): int;
  public function write(string $string): int;
}

interface UploadedFileInterface {
  public function getClientFilename(): ?string;
  public function getClientMediaType(): ?string;
  public function getError(): int;
  public function getSize(): int;
  public function getStream(): StreamInterface;
  public function moveTo(string $targetPath): bool;
}

interface UriInterface {
  public function __toString(): string;
  public function getAuthority(): string;
  public function getFragment(): string;
  public function getHost(): string;
  public function getPath(): string;
  public function getPort(): ?int;
  public function getQuery(): string;
  public function getScheme(): string;
  public function getUserInfo(): string;
  public function withFragment(string $fragment): this;
  public function withHost(string $host): this;
  public function withPath(string $path): this;
  public function withPort(?int $port): this;
  public function withQuery(string $query): this;
  public function withScheme(string $scheme): this;
  public function withUserInfo(string $user, ?string $password = null): this;
}
