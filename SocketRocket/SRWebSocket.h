//
//   Copyright 2012 Square Inc.
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//

#import <Foundation/Foundation.h>
#import <Security/SecCertificate.h>

typedef NS_ENUM(NSInteger, SRReadyState) {
    SRReadyStateConnecting = 0,
    SRReadyStateOpen = 1,
    SRReadyStateClosing = 2,
    SRReadyStateClosed = 3,
};

/**
 *  Legacy SRReadyState constants. These map directly to the new constants.
 */
extern SRReadyState const SR_CONNECTING;
extern SRReadyState const SR_OPEN;
extern SRReadyState const SR_CLOSING;
extern SRReadyState const SR_CLOSED;

typedef NS_ENUM(NSInteger, SRStatusCode) {
    SRStatusCodeNormal = 1000,
    SRStatusCodeGoingAway = 1001,
    SRStatusCodeProtocolError = 1002,
    SRStatusCodeUnhandledType = 1003,
    // 1004 reserved.
    SRStatusNoStatusReceived = 1005,
    // 1004-1006 reserved.
    SRStatusCodeInvalidUTF8 = 1007,
    SRStatusCodePolicyViolated = 1008,
    SRStatusCodeMessageTooBig = 1009,
};

extern NSString *const SRWebSocketErrorDomain;
extern NSString *const SRHTTPResponseErrorKey;

#pragma mark - SRWebSocketDelegate

@protocol SRWebSocketDelegate;

#pragma mark - SRWebSocket

@interface SRWebSocket : NSObject <NSStreamDelegate>

/**
 *  Set and retrieve the delegate.
 */
@property (nonatomic, weak) id <SRWebSocketDelegate> delegate;

/**
 *  The current state of the connection.
 */
@property (nonatomic, readonly) SRReadyState readyState;

/**
 *  The connection's endpoint.
 */
@property (nonatomic, readonly) NSURL *url;

@property (nonatomic, readonly) CFHTTPMessageRef receivedHTTPHeaders;

// Optional array of cookies (NSHTTPCookie objects) to apply to the connections
@property (nonatomic, readwrite) NSArray * requestCookies;

// This returns the negotiated protocol.
// It will be nil until after the handshake completes.
@property (nonatomic, readonly, copy) NSString *protocol;

/**
 *  This property determine whether NSString/NSData objects are copied before being sent. The default value of this property is YES. Set this property to NO for a minor performance optimization if you know you are sending objects that won't change before they are written.
 */
@property (nonatomic) BOOL sendDataSafely;

/**
 *  Set this to YES if you would like to allow invalid SSL certificates. This is not recommended and the default value is NO. In DEBUG mode, this property will default to YES.
 */
@property (nonatomic) BOOL allowInsecureConnections;

// Protocols should be an array of strings that turn into Sec-WebSocket-Protocol.
- (instancetype)initWithURLRequest:(NSURLRequest *)request protocols:(NSArray *)protocols allowsUntrustedSSLCertificates:(BOOL)allowsUntrustedSSLCertificates;
- (instancetype)initWithURLRequest:(NSURLRequest *)request protocols:(NSArray *)protocols;
- (instancetype)initWithURLRequest:(NSURLRequest *)request;

// Some helper constructors.
- (instancetype)initWithURL:(NSURL *)url protocols:(NSArray *)protocols allowsUntrustedSSLCertificates:(BOOL)allowsUntrustedSSLCertificates;
- (instancetype)initWithURL:(NSURL *)url protocols:(NSArray *)protocols;
- (instancetype)initWithURL:(NSURL *)url;

// Delegate queue will be dispatch_main_queue by default.
// You cannot set both OperationQueue and dispatch_queue.
- (void)setDelegateOperationQueue:(NSOperationQueue *)queue;
- (void)setDelegateDispatchQueue:(dispatch_queue_t)queue;

// By default, it will schedule itself on +[NSRunLoop SR_networkRunLoop] using defaultModes.
- (void)scheduleInRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;
- (void)unscheduleFromRunLoop:(NSRunLoop *)aRunLoop forMode:(NSString *)mode;

// SRWebSockets are intended for one-time-use only. Open should be called once and only once.
- (void)open;

- (void)close;
- (void)closeWithCode:(NSInteger)code reason:(NSString *)reason;

// Send a UTF8 String or Data.
- (void)send:(id)data;

- (void)sendString:(NSString *)message;

- (void)sendData:(NSData *)message;

/**
 *  Send data with an identifier in such a way that you will be notified when the write has completed.
 *
 *  @param message    The data.
 *  @param identifier The identifier.
 */
- (void)sendPartialData:(NSData *)message withIdentifier:(id)identifier;

// Send Data (can be nil) in a ping message.
- (void)sendPing:(NSData *)data;

@end

#pragma mark - SRWebSocketDelegate

@protocol SRWebSocketDelegate <NSObject>

@optional

- (void)webSocketDidOpen:(SRWebSocket *)webSocket;
- (void)webSocket:(SRWebSocket *)webSocket didFailWithError:(NSError *)error;
- (void)webSocket:(SRWebSocket *)webSocket didCloseWithCode:(NSInteger)code reason:(NSString *)reason wasClean:(BOOL)wasClean;
- (void)webSocket:(SRWebSocket *)webSocket didReceivePong:(NSData *)pongPayload;

// message will either be an NSString if the server is using text
// or NSData if the server is using binary.
- (void)webSocket:(SRWebSocket *)webSocket didReceiveMessage:(id)message;

- (void)webSocket:(SRWebSocket *)webSocket didReceiveString:(NSString *)message;
- (void)webSocket:(SRWebSocket *)webSocket didReceiveData:(NSData *)message;

- (void)webSocket:(SRWebSocket *)webSocket writeDidFinishWithIdentifier:(id)identifier;

@end

#pragma mark - NSURLRequest (CertificateAdditions)

@interface NSURLRequest (CertificateAdditions)

@property (nonatomic, retain, readonly) NSArray *SR_SSLPinnedCertificates;

@end

#pragma mark - NSMutableURLRequest (CertificateAdditions)

@interface NSMutableURLRequest (CertificateAdditions)

@property (nonatomic, retain) NSArray *SR_SSLPinnedCertificates;

@end

#pragma mark - NSRunLoop (SRWebSocket)

@interface NSRunLoop (SRWebSocket)

+ (NSRunLoop *)SR_networkRunLoop;

@end
