#ifndef CLIENT_GLOBALS_H
#define CLIENT_GLOBALS_H

#include <QString>
#include <QDateTime>

// User role types for authentication
enum class UserRole {
    Unknown = 0,
    Factory = 1,    // 工厂 (Factory)
    Expert = 2      // 专家 (Expert)
};

// Work order status types
enum class WorkOrderStatus {
    Created = 0,
    InProgress = 1,
    Completed = 2,
    Cancelled = 3
};

// Global constants
namespace ClientGlobals {
    // Server connection settings
    const QString DEFAULT_SERVER_HOST = "127.0.0.1";
    const int DEFAULT_SERVER_PORT = 12345;
    
    // User session info
    extern QString currentUser;
    extern UserRole currentUserRole;
    extern QString sessionToken;
    
    // Helper functions
    QString userRoleToString(UserRole role);
    UserRole userRoleFromString(const QString& roleStr);
    QString workOrderStatusToString(WorkOrderStatus status);
    WorkOrderStatus workOrderStatusFromString(const QString& statusStr);
}

#endif // CLIENT_GLOBALS_H