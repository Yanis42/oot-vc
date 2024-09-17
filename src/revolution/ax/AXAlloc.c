#include "revolution/ax.h"
#include "revolution/os.h"

#define STACK_MAX (AX_PRIORITY_MAX + 1)

static AXVPB* __AXStackHead[STACK_MAX];
static AXVPB* __AXStackTail[STACK_MAX + 4];
static AXVPB* __AXCallbackStack = NULL;

AXVPB* __AXGetStackHead(u32 prio) { return __AXStackHead[prio]; }

void __AXServiceCallbackStack(void) {
    AXVPB* it = __AXPopCallbackStack();

    while (it != NULL) {
        if (it->priority > AX_PRIORITY_FREE) {
            if (it->callback != NULL) {
                it->callback(it);
            }

            __AXRemoveFromStack(it);
            __AXPushFreeStack(it);
        }

        it = __AXPopCallbackStack();
    }
}

static inline void __AXInitVoiceStacks(void) {
    u32 prio;

    __AXCallbackStack = NULL;

    for (prio = 0; prio < STACK_MAX; prio++) {
        __AXStackHead[prio] = __AXStackTail[prio] = NULL;
    }
}

void __AXAllocInit(void) { __AXInitVoiceStacks(); }

void __AXAllocQuit(void) { __AXInitVoiceStacks(); }

void __AXPushFreeStack(AXVPB* vpb) {
    vpb->next = __AXStackHead[AX_PRIORITY_FREE];
    __AXStackHead[AX_PRIORITY_FREE] = vpb;
    vpb->priority = AX_PRIORITY_FREE;
}

static inline AXVPB* __AXPopFreeStack(void) {
    AXVPB* head = __AXStackHead[AX_PRIORITY_FREE];

    if (head != NULL) {
        __AXStackHead[AX_PRIORITY_FREE] = head->next;
    }

    return head;
}

void __AXPushCallbackStack(AXVPB* vpb) {
    vpb->next1 = __AXCallbackStack;
    __AXCallbackStack = vpb;
}

static inline AXVPB* __AXPopCallbackStack(void) {
    AXVPB* head = __AXCallbackStack;

    if (head != NULL) {
        __AXCallbackStack = head->next1;
    }

    return head;
}

void __AXRemoveFromStack(AXVPB* vpb) {
    u32 prio = vpb->priority;
    AXVPB* head = __AXStackHead[prio];
    AXVPB* tail = __AXStackTail[prio];

    if (head == tail) {
        __AXStackTail[prio] = NULL;
        __AXStackHead[prio] = NULL;
    } else if (vpb == head) {
        __AXStackHead[prio] = vpb->next;
        __AXStackHead[prio]->prev = NULL;
    } else if (vpb == tail) {
        __AXStackTail[prio] = vpb->prev;
        __AXStackTail[prio]->next = NULL;
    } else {
        head = vpb->prev;
        tail = vpb->next;
        head->next = tail;
        tail->prev = head;
    }
}

static inline void __AXPushStackHead(AXVPB* vpb, u32 prio) {
    vpb->next = __AXStackHead[prio];
    vpb->prev = NULL;

    if (vpb->next != NULL) {
        __AXStackHead[prio]->prev = vpb;
        __AXStackHead[prio] = vpb;
    } else {
        __AXStackTail[prio] = vpb;
        __AXStackHead[prio] = vpb;
    }

    vpb->priority = prio;
}

static inline AXVPB* __AXPopStackFromBottom(u32 prio) {
    AXVPB* vpb = NULL;

    if (__AXStackHead[prio] != NULL) {
        if (__AXStackHead[prio] == __AXStackTail[prio]) {
            vpb = __AXStackHead[prio];
            __AXStackTail[prio] = NULL;
            __AXStackHead[prio] = NULL;
        } else if (__AXStackTail[prio] != NULL) {
            vpb = __AXStackTail[prio];
            __AXStackTail[prio] = vpb->prev;
            __AXStackTail[prio]->next = NULL;
        }
    }

    return vpb;
}

void AXFreeVoice(AXVPB* vpb) {
    bool enabled = OSDisableInterrupts();

    __AXRemoveFromStack(vpb);

    if (vpb->pb.state == AX_VOICE_RUN) {
        vpb->depop = true;
    }

    __AXSetPBDefault(vpb);
    __AXPushFreeStack(vpb);

    OSRestoreInterrupts(enabled);
}

AXVPB* AXAcquireVoice(u32 prio, AXVoiceCallback callback, u32 userContext) {
    bool enabled = OSDisableInterrupts();
    AXVPB* vpb = __AXPopFreeStack();

    if (vpb == NULL) {
        u32 i;

        for (i = AX_PRIORITY_MIN; i < prio; i++) {
            vpb = __AXPopStackFromBottom(i);

            if (vpb != NULL) {
                if (vpb->pb.state == AX_VOICE_RUN) {
                    vpb->depop = true;
                }

                if (vpb->callback != NULL) {
                    vpb->callback(vpb);
                }

                break;
            }
        }
    }

    if (vpb != NULL) {
        __AXPushStackHead(vpb, prio);
        vpb->callback = callback;
        vpb->userContext = userContext;
        __AXSetPBDefault(vpb);
    }

    OSRestoreInterrupts(enabled);
    return vpb;
}

void AXSetVoicePriority(AXVPB* vpb, u32 prio) {
    bool enabled = OSDisableInterrupts();

    __AXRemoveFromStack(vpb);
    __AXPushStackHead(vpb, prio);

    OSRestoreInterrupts(enabled);
}
